#ifndef __T2BCORR_H__
#define __T2BCORR_H__

#include "t2b.h"

class T2Bcorr {
	const int c1[C1_LEN] = C1_INIT;
	const int c2[C2_LEN] = C2_INIT;
	const int c3[C3_LEN] = C3_INIT;
	const int c4[C4_LEN] = C4_INIT;
	const int c5[C5_LEN] = C5_INIT;
	const int c6[C6_LEN] = C6_INIT;

	int c1_int[C1_LEN];
	int c2_int[C2_LEN];
	int c3_int[C3_LEN];
	int c4_int[C4_LEN];
	int c5_int[C5_LEN];
	int c6_int[C6_LEN];

	int tcorr;
	int corr_i;

	int locked_min = 0;
	int locked_max = 0;
	int pos_min;
	int pos_max;

	private:
		int find_max_index(int *a, int len);
		int find_min_index(int *a, int len);
		int mul_inv(int a, int b);
		int chinese_remainder(int *n, int *a, int len);
		int position_difference(int pos_new, int pos_old);
		void calculate_position(void);

	public:
		T2Bcorr(int correlation_time);
		void reset(void);
		bool step(int chip);
		int get_position(void);
};

#endif
