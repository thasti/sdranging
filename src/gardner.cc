#include <iostream>
#include <complex>
#include "gardner.h"

Gardner::Gardner(float wn, float zeta, int osf) {
	float gain = 1.0/((1.0/M_PI)*osf);	// gardner error gain is ~4, loop runs at 1/osf
	gardner_osf = osf;

	phase_inc = 1.0/osf;
	phase_acc = 0;
	integrator = 0;

	g1 = (1-expf(-2*zeta*wn))/gain;
	g2 = (1+expf(-2*zeta*wn)-2*expf(-zeta*wn)*cosf(wn*sqrtf(1-zeta*zeta)))/gain;

	printf("%f %f\n", g1, g2);

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
		integrator = integrator + e;
		phase_acc = g1 * e + g2 * integrator;
		last_bit = input > 0.0;
	}
	/* update phase accumulator */
	phase_acc = phase_acc + phase_inc;
	
	/* update sample buffer, first element contains oldest value */
	for (i = 0; i < (gardner_osf-1); i++) {
		spl_buf[i] = spl_buf[i+1];
	}
	spl_buf[gardner_osf-1] = input;

	return new_bit_ready;
}

bool Gardner::get_last_bit(void) {
	return last_bit;
}
