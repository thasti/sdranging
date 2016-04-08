#ifndef __RANGECALC_H__
#define __RANGECALC_H__

#define RANGECALC_FRACTIONAL_LAMBDA	0.9
#define RANGECALC_OFFSET_LAMBDA		0.95
#define SPEED_OF_LIGHT			299792.458

class RangeCalc {
	int samp_rate;
	int chip_spls;
	long code_length_steps;
	long spl_position;
	int spl_offset;
	int tcorr;

	float spl_offset_frac;
	float spl_range;

	private:
		int position_difference(int pos_new, int pos_old);
	public:
		RangeCalc(int fs, int spls_per_chip, int correlation_chips);
		void step(void);
		void new_offset_position(int position);
		void new_range_position(int position);
		void preset_offset_average(void);
		float get_offset(void);
		float get_range(void);

};

#endif
