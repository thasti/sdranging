#include "t2bgen.h"

int T2Bgen::get_position(void) {
	return pos;
}

int T2Bgen::step(void) {
	int out = 2*c1[pos % C1_LEN] + c2[pos % C2_LEN] - c3[pos % C3_LEN] - c4[pos % C4_LEN] + c5[pos % C5_LEN] - c6[pos % C6_LEN];
	if (out > 0) {
		out = +1;
	} else {
		out = -1;
	}

	pos++;
	if (pos == SEQ_PERIOD) {
		pos = 0;
	}

	return out;
}
