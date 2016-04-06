#ifndef __GARDNER_H__
#define __GARDNER_H__

class Gardner {
	int gardner_osf;

	float phase_inc;
	float phase_acc;
	float integrator;

	float g1;
	float g2;

	float *spl_buf;

	bool last_bit;

	public:
		Gardner(float wn, float zeta, int osf);
		bool step(float input);
		bool get_last_bit(void);
		bool is_locked(void);

};

#endif
