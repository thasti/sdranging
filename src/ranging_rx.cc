#include <complex>
#include <iostream>
#include <fstream>
#include <libbladeRF.h>
#include <liquid/liquid.h>
#include "bladerf.h"
#include "ranging_rx.h"
#include "costas.h"
#include "gardner.h"
#include "t2bcorr.h"
#include "rangecalc.h"

extern int done;
extern int calibration_running;

void process_corr_result(Costas &costas, T2Bcorr &t2bcorr, RangeCalc &rangecalc) {
	int t2b_corr_pos = 0;
	static bool t2b_corr_locked = 0;

	t2b_corr_pos = t2bcorr.get_position();
	if (calibration_running) {
		if (t2bcorr.is_locked()) {
			rangecalc.new_offset_position(t2b_corr_pos);
			if (!t2b_corr_locked) {
				/* on first lock, preset average offset */
				rangecalc.preset_offset_average();
			}
			t2b_corr_locked = 1;
		} else {
			t2b_corr_locked = 0;
		}
	} else {
		if (t2bcorr.is_locked()) {
			rangecalc.new_range_position(t2b_corr_pos);
		}
		
	}
	printf("Done. Costas Lock: %d, T2B Lock: %d, Sample offset: %.2f, Range: %.2f km\n", 
			costas.is_locked(), 
			t2bcorr.is_locked(), 
			rangecalc.get_offset(),
			rangecalc.get_range());
}

void check_calibration_finish(struct bladerf *dev) {
	static bool calibration_already_finished = 0;
    	int status;
	
	if (!calibration_already_finished) {
		if (!calibration_running) {
			calibration_already_finished = 1;
			status = bladerf_set_frequency(dev, BLADERF_MODULE_RX, BLADERF_RX_MEAS_FREQUENCY);
    			if (status != 0) {
        			fprintf(stderr, "Failed to set RX measurement frequency, %s\n",
            			bladerf_strerror(status));
			}
			status = bladerf_set_frequency(dev, BLADERF_MODULE_TX, BLADERF_TX_MEAS_FREQUENCY);
    			if (status != 0) {
        			fprintf(stderr, "Failed to set TX measurement frequency, %s\n",
            			bladerf_strerror(status));
			}
			status = bladerf_set_loopback(dev, BLADERF_LB_NONE);
			if (status != 0) {
        			fprintf(stderr, "Failed to disable loopback mode, %s\n",
            			bladerf_strerror(status));
			}
    		}
	}
}

void rx_thread(struct bladerf *dev) {
	int16_t *rx_samples = NULL;
	std::complex<float> *rx_samples_cplx = NULL;
	const unsigned int samples_len = RX_BUFLEN_SPLS; /* May be any (reasonable) size */
	int i, j, status;
	
	/* fixed complex NCO for intentional zero-frequency offset correction */
	std::complex<float> complex_i(0.0, 1.0);
	std::complex<float> tx_nco_out;
	std::complex<float> tx_nco_in;
	nco_crcf rx_nco;

	/* decimating FIR channel filter */
	firdecim_crcf chfilt;
	int chfilt_len = estimate_req_filter_len(RX_CHFILT_TRANSITION, RX_CHFILT_AS);
	float chfilt_h[chfilt_len];
	float chfilt_sum = 0;
	std::complex<float> chfilt_out;

	/* limiter */
	float limiter_value = 1;

	/* carrier phase recovery costas loop */
	Costas costas(RX_COSTAS_WN, RX_COSTAS_ZETA);
	std::complex<float> costas_out;

	/* matched filter */
	firfilt_rrrf matched_filter;
	float matched_filter_h[MFILT_SPLS_PER_SYM];
	float matched_filter_out;
	float matched_filter_sum = 0;

	/* bit clock recovery gardner loop */
	Gardner gardner = Gardner(RX_GARDNER_WN, RX_GARDNER_ZETA, RX_GARDNER_SPLS_PER_SYM);
	bool gardner_new_bit = 0;

	/* T2B correlator for sequence position estimation */
	T2Bcorr t2bcorr = T2Bcorr(RX_T2B_CORR_CHIPS);
	bool t2b_corr_finished = 0;

	/* range tracker */
	RangeCalc rangecalc = RangeCalc(RX_GARDNER_FS, RX_GARDNER_SPLS_PER_SYM, RX_T2B_CORR_CHIPS);

    std::ofstream outfile("range_data.csv",std::ios_base::binary);

	/* Allocate a buffer to store received samples in */
	rx_samples = (int16_t *) malloc(samples_len * 2 * sizeof(int16_t));
	if (rx_samples == NULL) {
		perror("Cannot allocate RX buffer.\n");
		return;
	}
	rx_samples_cplx = (std::complex<float> *) malloc(samples_len * sizeof(std::complex<float>));
	if (rx_samples_cplx == NULL) {
		perror("Cannot allocate complex RX buffer.\n");
		free(rx_samples);
		return;
	}

	/* create and setup fixed NCO */
	rx_nco = nco_crcf_create(LIQUID_NCO);
	nco_crcf_set_phase(rx_nco, 0.0f);
	nco_crcf_set_frequency(rx_nco, RX_OFFSET_FREQ);
	
	/* design and normalize a channel filter */
	liquid_firdes_kaiser(chfilt_len, RX_CHFILT_CUTOFF, RX_CHFILT_AS, RX_CHFILT_MU, chfilt_h);
	for (i = 0; i < chfilt_len; i++) {
		chfilt_sum = chfilt_sum + chfilt_h[i];
	}
	for (i = 0; i < chfilt_len; i++) {
		chfilt_h[i] = chfilt_h[i] / chfilt_sum;
	}
	chfilt = firdecim_crcf_create(RX_CHFILT_DECIM, chfilt_h, chfilt_len);
	
	/* design and normalize matched filter */
	for (i = 0; i < MFILT_SPLS_PER_SYM; i++) {
		matched_filter_h[i] = sinf(M_PI * i / MFILT_SPLS_PER_SYM);
		matched_filter_sum += matched_filter_h[i];
	}
	for (i = 0; i < MFILT_SPLS_PER_SYM; i++) {
		matched_filter_h[i] = matched_filter_h[i] / matched_filter_sum;
	}
	matched_filter = firfilt_rrrf_create(matched_filter_h, MFILT_SPLS_PER_SYM);

	/* run processing */
	j = 0;
	while(!done) {
		status = bladerf_sync_rx(dev, rx_samples, samples_len, NULL, 5000);
		if (status == 0) {
			for (i = 0; i < RX_BUFLEN_SPLS; i++) {
				rx_samples_cplx[i] = 
					(float)(rx_samples[2*i])/RX_IQ_MAX + complex_i * (float)(rx_samples[2*i+1])/RX_IQ_MAX;
			}
			nco_crcf_mix_block_down(rx_nco, rx_samples_cplx, rx_samples_cplx, samples_len);
			for (i = 0; i < RX_BUFLEN_SPLS; i+=RX_CHFILT_DECIM, j++) {
				firdecim_crcf_execute(chfilt, &rx_samples_cplx[i], &chfilt_out);
				if (abs(chfilt_out) >= limiter_value) {
					limiter_value = abs(chfilt_out);
				} else {
					limiter_value = RX_LIM_LAMBDA * limiter_value + (1.0-RX_LIM_LAMBDA) * abs(chfilt_out);
				}
				chfilt_out = chfilt_out / limiter_value;
				costas_out = costas.step(chfilt_out);

				firfilt_rrrf_push(matched_filter, costas_out.imag());
				firfilt_rrrf_execute(matched_filter, &matched_filter_out);

			
				gardner_new_bit = gardner.step(matched_filter_out);

				rangecalc.step();

				if (gardner_new_bit) {
					/* get new available bit, process */
					gardner_new_bit = gardner.get_last_bit();
					t2b_corr_finished = t2bcorr.step((int)(gardner_new_bit)*2 - 1);
					if (t2b_corr_finished) {
						process_corr_result(costas, t2bcorr, rangecalc);
						check_calibration_finish(dev);
                        if (t2bcorr.is_locked() && (rangecalc.get_range() > 0.0f)) {
                            outfile << rangecalc.get_range() << std::endl;
                        }
					}
				}
			}
			
		} else {
			printf("bladerx_sync_rx() timed out.!\n");
		}
	}
	outfile.close();

	free(rx_samples);

}
