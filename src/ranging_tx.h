#ifndef __RANGING_TX_H__
#define __RANGING_TX_H__

/* sample and symbol rate definition
 * constraints: buffer length is integer multiple of chip length
 * sample rate is integer multiple of symbol rate
 */
#define TX_FS		2000000
#define TX_OSF		300
#define TX_FC		(TX_FS/TX_OSF)
#define TX_BUFLEN_CHIPS	100
#define TX_BUFLEN_SPLS	(TX_OSF*TX_BUFLEN_CHIPS)

#define TX_IQ_MAX	1023

#define TX_OFFSET_FREQ	(2.0 * M_PI * 250e3 / TX_FS)

void tx_thread(struct bladerf *dev);

#endif
