#ifndef __RANGING_RX_H__
#define __RANGING_RX_H__

/* ensure that RX sample rate is the same as the TX sample rate */
#define RX_FS		2000000
#define RX_OSF_NOM	300
#define RX_FC_NOM	(RX_FS/RX_OSF_NOM)
#define RX_BUFLEN_CHIPS	100
#define RX_BUFLEN_SPLS	(RX_OSF_NOM*RX_BUFLEN_CHIPS)

#define RX_IQ_MAX	2048.0f

/* fixed NCO phase increment (zero-frequency offset) */
#define RX_OFFSET_FREQ	(2.0 * M_PI * 250e3 / RX_FS)

/* channel filter parameters */
#define RX_CHFILT_CUTOFF	(10.0e3/RX_FS)
#define RX_CHFILT_TRANSITION	(5.0e3/RX_FS)
#define RX_CHFILT_AS		40.0
#define RX_CHFILT_MU		0.0f
#define RX_CHFILT_DECIM		4

/* limiter time constant */
#define RX_LIM_LAMBDA		0.999

/* costas loop parameters */
#define RX_COSTAS_FS		(RX_FS/RX_CHFILT_DECIM)
#define RX_COSTAS_WN		(2*M_PI*(RX_FC_NOM/100)/RX_COSTAS_FS)
#define RX_COSTAS_ZETA		0.7071f

/* matched filter parameters */
#define MFILT_SPLS_PER_SYM	(RX_OSF_NOM/RX_CHFILT_DECIM)

/* gardner loop parameters */
#define RX_GARDNER_SPLS_PER_SYM	(RX_OSF_NOM/RX_CHFILT_DECIM)
#define RX_GARDNER_FS		(RX_FS/RX_CHFILT_DECIM)
#define RX_GARDNER_WN		(2*M_PI*(RX_FC_NOM/20)/RX_GARDNER_FS)
#define RX_GARDNER_ZETA		1.0

/* T2B correlator parameters */
#define RX_T2B_CORR_CHIPS	10e3

void rx_thread(struct bladerf *dev);

#endif
