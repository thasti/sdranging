#ifndef __T2BGEN_H__
#define __T2BGEN_H__

#define C1_LEN	2
#define C2_LEN	7
#define C3_LEN	11
#define C4_LEN	15
#define C5_LEN	19
#define C6_LEN	23
#define SEQ_PERIOD	(C1_LEN*C2_LEN*C3_LEN*C4_LEN*C5_LEN*C6_LEN)

class T2Bgen {
	int c1[C1_LEN] = {+1, -1};
	int c2[C2_LEN] = {+1, +1, +1, -1, -1, +1, -1};
	int c3[C3_LEN] = {+1, +1, +1, -1, -1, -1, +1, -1, +1, +1, -1};
	int c4[C4_LEN] = {+1, +1, +1, +1, -1, -1, -1, +1, -1, -1, +1, +1, -1, +1, -1};
	int c5[C5_LEN] = {+1, +1, +1, +1, -1, +1, -1, +1, -1, -1, -1, -1, +1, +1, -1, +1, +1, -1, -1};
	int c6[C6_LEN] = {+1, +1, +1, +1, +1, -1, +1, -1, +1, +1, -1, -1, +1, +1, -1, -1, +1, -1, +1, -1, -1, -1, -1};

	int pos = 0;
	
	public:
		int get_position(void);
		int step(void);
};

#endif
