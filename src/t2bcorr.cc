#include "t2bcorr.h"
#include "t2b.h"
#include <climits>

T2Bcorr::T2Bcorr(int correlation_time) {
	reset();
	tcorr = correlation_time;
	locked_min = 0;
	locked_max = 0;
	pos_max = 0;
	pos_min = 0;
}

void T2Bcorr::reset(void) {
	int i;

	/* reset correlation counter to 0 */
	corr_i = 0;

	/* reset all the integrators */
	for (i = 0; i < C1_LEN; i++) {
		c1_int[i] = 0;
	}
	for (i = 0; i < C2_LEN; i++) {
		c2_int[i] = 0;
	}
	for (i = 0; i < C3_LEN; i++) {
		c3_int[i] = 0;
	}
	for (i = 0; i < C4_LEN; i++) {
		c4_int[i] = 0;
	}
	for (i = 0; i < C5_LEN; i++) {
		c5_int[i] = 0;
	}
	for (i = 0; i < C6_LEN; i++) {
		c6_int[i] = 0;
	}
}

bool T2Bcorr::step(int chip) {
	int i;

	for (i = 0; i < C1_LEN; i++) {
		c1_int[i] = c1_int[i] + chip * c1[(corr_i + i) % C1_LEN];
	}
	for (i = 0; i < C2_LEN; i++) {
		c2_int[i] = c2_int[i] + chip * c2[(corr_i + i) % C2_LEN];
	}
	for (i = 0; i < C3_LEN; i++) {
		c3_int[i] = c3_int[i] - chip * c3[(corr_i + i) % C3_LEN];
	}
	for (i = 0; i < C4_LEN; i++) {
		c4_int[i] = c4_int[i] - chip * c4[(corr_i + i) % C4_LEN];
	}
	for (i = 0; i < C5_LEN; i++) {
		c5_int[i] = c5_int[i] + chip * c5[(corr_i + i) % C5_LEN];
	}
	for (i = 0; i < C6_LEN; i++) {
		c6_int[i] = c6_int[i] - chip * c6[(corr_i + i) % C6_LEN];
	}

	corr_i++;

	if (corr_i >= tcorr) {
		calculate_position();
		return true;
	} else {
		return false;
	}

}

int T2Bcorr::find_max_index(int *a, int len) {
	int i;
	int max = -INT_MAX;
	int max_idx = -1;

	for (i = 0; i < len; i++) {
		if (a[i] > max) {
			max_idx = i;
			max = a[i];
		}
	}

	return max_idx;
}

int T2Bcorr::find_min_index(int *a, int len) {
	int i;
	int min = INT_MAX;
	int min_idx = -1;

	for (i = 0; i < len; i++) {
		if (a[i] < min) {
			min_idx = i;
			min = a[i];
		}
	}

	return min_idx;
}

// returns x where (a * x) % b == 1
int T2Bcorr::mul_inv(int a, int b)
{
	int b0 = b, t, q;
	int x0 = 0, x1 = 1;
	if (b == 1) return 1;
	while (a > 1) {
		q = a / b;
		t = b, b = a % b, a = t;
		t = x0, x0 = x1 - q * x0, x1 = t;
	}
	if (x1 < 0) x1 += b0;
	return x1;
}
 

int T2Bcorr::chinese_remainder(int *n, int *a, int len) {
	int p, i, prod = 1, sum = 0;
 
	for (i = 0; i < len; i++) prod *= n[i];
 
	for (i = 0; i < len; i++) {
		p = prod / n[i];
		sum += a[i] * mul_inv(p, n[i]) * p;
	}
 
	return sum % prod;
}

int T2Bcorr::position_difference(int pos_new, int pos_old) {	
	if (pos_new >= pos_old) {
		return pos_new - pos_old;
	} else {
		return -pos_old + SEQ_PERIOD + pos_new;
	}
}	

void T2Bcorr::calculate_position(void) {
	int moduli[6] = {C1_LEN, C2_LEN, C3_LEN, C4_LEN, C5_LEN, C6_LEN};
	int max_idx[6];
	int min_idx[6];
	int pos_min_new;
	int pos_max_new;

	max_idx[0] = find_max_index(c1_int, C1_LEN);
	max_idx[1] = find_max_index(c2_int, C2_LEN);
	max_idx[2] = find_max_index(c3_int, C3_LEN);
	max_idx[3] = find_max_index(c4_int, C4_LEN);
	max_idx[4] = find_max_index(c5_int, C5_LEN);
	max_idx[5] = find_max_index(c6_int, C6_LEN);
	
	min_idx[0] = find_min_index(c1_int, C1_LEN);
	min_idx[1] = find_min_index(c2_int, C2_LEN);
	min_idx[2] = find_min_index(c3_int, C3_LEN);
	min_idx[3] = find_min_index(c4_int, C4_LEN);
	min_idx[4] = find_min_index(c5_int, C5_LEN);
	min_idx[5] = find_min_index(c6_int, C6_LEN);

	reset();

	pos_max_new = chinese_remainder(moduli, max_idx, 6);
	pos_min_new = chinese_remainder(moduli, min_idx, 6);

	/* prefer currently locked correlators */
	if (locked_max) {
		if (position_difference(pos_max_new, pos_max) != tcorr) {
			locked_max = 0;
		}
	} else if (locked_min) {
		if (position_difference(pos_min_new, pos_min) != tcorr) {
			locked_min = 0;
		}
	} else {
		if (position_difference(pos_max_new, pos_max) == tcorr) {
			locked_max = 1;
		} else if (position_difference(pos_min_new, pos_min) == tcorr) {
			locked_min = 1;
		}
	}
	
	pos_max = pos_max_new;
	pos_min = pos_min_new;
	
}

int T2Bcorr::get_position(void) {
		if (locked_min) {
				return pos_min;
		} else if (locked_max) {
				return pos_max;
		} else {
				return 0;
		}
}

bool T2Bcorr::is_locked(void) {
	return locked_min || locked_max;
}
