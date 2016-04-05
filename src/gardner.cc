#include <iostream>
#include <complex>
#include "gardner.h"

Gardner::Gardner(int osf) {
	phase_inc = 1.0/osf;
	phase_acc = 0;
	ld_avg = 0;
	lpf = 0;
	gardner_osf = osf;

	spl_buf = (float *)malloc(osf * sizeof(float));
	if (spl_buf == NULL) {
		perror("Cannot allocate Gardner sample buffer.\n");
		exit(-1);
	}
}

/* 
 * step() - Gardner loop update function
 * 
 * returns whether a new output bit has been recovered
 */
bool Gardner::step(float input) {
	bool new_bit_ready = false;
	float e;
	int i;

	/* calculate loop error */
	if (phase_acc >= 1.0) {
		new_bit_ready = true;
		e = (input - spl_buf[0]) * spl_buf[gardner_osf/2];
		lpf = GARDNER_LPF_LAMBDA * lpf + (1-GARDNER_LPF_LAMBDA) * e;
		ld_avg = GARDNER_LD_LAMBDA * ld_avg + (1-GARDNER_LD_LAMBDA) * fabs(lpf);
		phase_acc = GARDNER_LOOP_K * lpf;
		last_bit = input > 0.0;
	}
	/* update phase accumulator */
	phase_acc = phase_acc + phase_inc;
	
	/* update sample buffer, first element contains oldest value */
	for (i = 0; i < gardner_osf-1; i++) {
		spl_buf[i] = spl_buf[i+1];
	}
	spl_buf[gardner_osf-1] = input;

	return new_bit_ready;
}

bool Gardner::get_last_bit(void) {
	return last_bit;
}

bool Gardner::is_locked(void) {
	return ld_avg < GARDNER_LD_THR;
}
