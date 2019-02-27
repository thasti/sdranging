#ifndef __BLADERF_H__
#define __BLADERF_H__

#define BLADERF_RX_CALIB_FREQUENCY	2399830000
#define BLADERF_TX_CALIB_FREQUENCY	2399830000

// NB transponder setup
//#define BLADERF_RX_MEAS_FREQUENCY	739330000
//#define BLADERF_TX_MEAS_FREQUENCY	2399830000

// WB transponder setup
#define BLADERF_RX_MEAS_FREQUENCY	747500000
#define BLADERF_TX_MEAS_FREQUENCY	2408000000

struct module_config {
    bladerf_module module;

    unsigned int frequency;
    unsigned int bandwidth;
    unsigned int samplerate;

    /* Gains */
    bladerf_lna_gain rx_lna;
    int vga1;
    int vga2;
};

int configure_module(struct bladerf *dev, struct module_config *c);
int init_sync(struct bladerf *dev);

#endif
