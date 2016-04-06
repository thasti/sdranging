#ifndef __T2BGEN_H__
#define __T2BGEN_H__

#include "t2b.h"

class T2Bgen {
	const int c1[C1_LEN] = C1_INIT;
	const int c2[C2_LEN] = C2_INIT;
	const int c3[C3_LEN] = C3_INIT;
	const int c4[C4_LEN] = C4_INIT;
	const int c5[C5_LEN] = C5_INIT;
	const int c6[C6_LEN] = C6_INIT;

	int pos = 0;
	
	public:
		int get_position(void);
		int step(void);
};

#endif
