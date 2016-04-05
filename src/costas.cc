#include <complex>
#include "costas.h"

Costas::Costas(float wn, float zeta) {
	phase_acc = 0;
	phase_inc = 0;
	integrator = 0;
	ld_avg = 1;

	g1 = (1-expf(-2*zeta*wn))/gain;
	g2 = (1+expf(-2*zeta*wn)-2*expf(-zeta*wn)*cosf(wn*sqrtf(1-zeta*zeta)))/gain;
}

std::complex<float> Costas::step(std::complex<float> input) {
	std::complex<float> complex_i(0.0, 1.0);
	std::complex<float> tmp;
	float phase_err;
	/* calculate NCO output */
	tmp = cosf(phase_acc) + complex_i*sinf(phase_acc);
	/* mix NCO with input */
	tmp = tmp * input;
	/* I * Q phase detector */
	phase_err = tmp.real() * tmp.imag();
	/* update integrator */
	integrator = integrator + phase_err;
	/* calculate new phase increment */
	phase_inc = g1 * phase_err + g2 * integrator;
	/* advance NCO */
	phase_acc = phase_acc + phase_inc;
	/* keep NCO modulo 2 * pi */
	if (phase_acc > 2*M_PI) {
		phase_acc -= 2*M_PI;
	} else if (phase_acc < -2*M_PI) {
		phase_acc += 2*M_PI;
	}
	
	ld_avg = COSTAS_LD_LAMBDA * ld_avg + (1.0-COSTAS_LD_LAMBDA) * fabs(tmp.real());

	return tmp;
}

bool Costas::is_locked() {
	return ld_avg < COSTAS_LD_THR;
}
