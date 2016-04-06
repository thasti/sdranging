#ifndef __T2B_H__
#define __T2B_H__

#define C1_LEN	2
#define C2_LEN	7
#define C3_LEN	11
#define C4_LEN	15
#define C5_LEN	19
#define C6_LEN	23
#define SEQ_PERIOD	(C1_LEN*C2_LEN*C3_LEN*C4_LEN*C5_LEN*C6_LEN)
	
#define C1_INIT {+1, -1}
#define C2_INIT {+1, +1, +1, -1, -1, +1, -1}
#define C3_INIT {+1, +1, +1, -1, -1, -1, +1, -1, +1, +1, -1}
#define C4_INIT {+1, +1, +1, +1, -1, -1, -1, +1, -1, -1, +1, +1, -1, +1, -1}
#define C5_INIT {+1, +1, +1, +1, -1, +1, -1, +1, -1, -1, -1, -1, +1, +1, -1, +1, +1, -1, -1}
#define C6_INIT {+1, +1, +1, +1, +1, -1, +1, -1, +1, +1, -1, -1, +1, +1, -1, -1, +1, -1, +1, -1, -1, -1, -1}

#endif
