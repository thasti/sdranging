#include <complex>

#define COSTAS_LD_THR	0.1
#define COSTAS_LD_LAMBDA	0.9995

class Costas {
	float g1, g2;
	float integrator;
	float phase_acc;
	float phase_inc;
	float ld_avg;

	public:
		Costas(float wn, float zeta);
		std::complex<float> step(std::complex<float> input);
		bool is_locked(void);
};
