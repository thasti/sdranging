#ifndef __GARDNER_H__
#define __GARDNER_H__

#define GARDNER_LPF_LAMBDA	0.92
#define GARDNER_LOOP_K		0.45

#define GARDNER_LD_LAMBDA	0.9995
#define GARDNER_LD_THR		0.1

class Gardner {
	int gardner_osf;

	float phase_inc;
	float phase_acc;
	float lpf;
	float ld_avg;

	float *spl_buf;

	bool last_bit;

	public:
		Gardner(int osf);
		bool step(float input);
		bool get_last_bit(void);
		bool is_locked(void);

};

#endif
