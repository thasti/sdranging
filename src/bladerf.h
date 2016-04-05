#ifndef __BLADERF_H__
#define __BLADERF_H__

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
