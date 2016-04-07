#ifndef __RANGECALC_H__
#define __RANGECALC_H__

#define RANGECALC_FRACTIONAL_LAMBDA	0.9
#define RANGECALC_OFFSET_LAMBDA		0.9

class RangeCalc {
	int chip_spls;
	long code_length_steps;
	long spl_position;
	int spl_offset;
	int tcorr;

	float spl_offset_frac;

	private:
		int position_difference(int pos_new, int pos_old);
	public:
		RangeCalc(int fs, int spls_per_chip, int correlation_chips);
		void step(void);
		void new_position(int position);
		float get_offset(void);
};

#endif
