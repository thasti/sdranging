#include "rangecalc.h"
#include "t2b.h"
#include <stdio.h>

RangeCalc::RangeCalc(int fs, int spls_per_chip, int correlation_chips) {
	code_length_steps = spls_per_chip * SEQ_PERIOD;
	chip_spls = spls_per_chip;
	spl_position = 0;
	spl_offset = 0;
	spl_offset_frac = 0.0;
	tcorr = correlation_chips;
}

void RangeCalc::step(void) {
	spl_position++;
	if (spl_position >= code_length_steps) {
		spl_position = 0;
	}
}

int RangeCalc::position_difference(int pos_new, int pos_old) {	
	pos_new = pos_new % code_length_steps;
	pos_old = pos_old % code_length_steps;

	if (pos_new >= pos_old) {
		return pos_new - pos_old;
	} else {
		return -pos_old + code_length_steps + pos_new;
	}
}	

void RangeCalc::new_position(int position) {
	long rxd_spl_position;

	rxd_spl_position = position * chip_spls;
	spl_offset = position_difference(rxd_spl_position + tcorr * chip_spls, spl_position);
	spl_offset_frac = 
		RANGECALC_OFFSET_LAMBDA * spl_offset_frac + 
		(1.0 - RANGECALC_OFFSET_LAMBDA) * spl_offset;
}

float RangeCalc::get_offset(void) {
	return spl_offset_frac;
}

