#include <complex>
#include <iostream>
#include <fstream>
#include <libbladeRF.h>
#include <liquid/liquid.h>
#include "ranging_rx.h"
#include "costas.h"

extern int done;

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
	int costas_locked = 0;

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

	std::ofstream out("tmp.dat",std::ios_base::binary);
	float write_i;
	float write_q;

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

				if (!costas_locked && costas.is_locked()) {
					costas_locked = 1;
					printf("Costas locked at %d!\n", j);
				}
				if (costas_locked && !costas.is_locked()) {
					costas_locked = 0;
					printf("Costas unlocked at %d!\n", j);
				}

				write_i = costas_out.real();
				write_q = costas_out.imag();

				out.write((char *)&write_i,sizeof(float));
				out.write((char *)&write_q,sizeof(float));
			}
			
		} else {
			printf("bladerx_sync_rx() timed out.!\n");
		}
	}
	out.close();

	free(rx_samples);

}
