#include <complex>
#include <libbladeRF.h>
#include <liquid/liquid.h>
#include "ranging_tx.h"
#include "t2bgen.h"

extern int done;

void tx_thread(struct bladerf *dev) {
	int16_t *tx_samples[2] = {NULL, NULL};
	const unsigned int samples_len = TX_BUFLEN_SPLS; /* May be any (reasonable) size */
	int status;
	int active_buffer = 0;

	std::complex<float> complex_i(0.0, 1.0);
	std::complex<float> tx_nco_out;
	std::complex<float> tx_nco_in;
	nco_crcf tx_nco;

	float pulse[TX_OSF];
	int h, j, k, spl_cnt;

	T2Bgen t2bgen = T2Bgen();

	int chip = t2bgen.step();
	int chip_d = -1;
	float tx_chip_phase = 0;

	for (h = 0; h < TX_OSF; h++) {
		pulse[h] = sinf(M_PI * h / TX_OSF);
	}

	/* Allocate a buffer to prepare transmit data in */
	tx_samples[0] = (int16_t *) malloc(samples_len * 2 * sizeof(int16_t));
	if (tx_samples[0] == NULL) {
		perror("Cannot allocate TX buffer.");
		return;
	}
	tx_samples[1] = (int16_t *) malloc(samples_len * 2 * sizeof(int16_t));
	if (tx_samples[1] == NULL) {
		perror("Cannot allocate TX buffer.");
		free(tx_samples[0]);
		return;
	}
	
	tx_nco = nco_crcf_create(LIQUID_NCO);
	nco_crcf_set_phase(tx_nco, 0.0f);
	nco_crcf_set_frequency(tx_nco, TX_OFFSET_FREQ);

	while (!done) {
		if (active_buffer == 0) {
			active_buffer = 1;
		} else {
			active_buffer = 0;
		}
		spl_cnt = 0;
		for (j = 0; j < TX_BUFLEN_CHIPS; j++) {
			for (k = 0; k < TX_OSF; k++) {
				if (chip_d < 0 && chip < 0) {
					tx_chip_phase = -1;
				} else if (chip_d < 0 && chip > 0) {
					if (k < TX_OSF/2) {
						tx_chip_phase = -pulse[k+TX_OSF/2];
					} else {
						tx_chip_phase = pulse[k-TX_OSF/2];
					}
				} else if (chip_d > 0 && chip < 0) {
					if (k < TX_OSF/2) {
						tx_chip_phase = pulse[k+TX_OSF/2];
					} else {
						tx_chip_phase = -pulse[k-TX_OSF/2];
					}
				} else if (chip_d > 0 && chip > 0) {
					tx_chip_phase = 1;
				}
				tx_nco_in = complex_i*tx_chip_phase;
				nco_crcf_step(tx_nco);
				nco_crcf_mix_up(tx_nco, tx_nco_in, &tx_nco_out);
				(tx_samples[active_buffer])[spl_cnt++] = tx_nco_out.real() * TX_IQ_MAX;
				(tx_samples[active_buffer])[spl_cnt++] = tx_nco_out.imag() * TX_IQ_MAX;
			}
			chip_d = chip;
			chip = t2bgen.step();
		}

		status = bladerf_sync_tx(dev, tx_samples[active_buffer], samples_len, NULL, 5000);
		if (status != 0) {
			printf("bladerx_sync_tx() timed out.!\n");
		}
	}

	free(tx_samples[0]);
	free(tx_samples[1]);

}
