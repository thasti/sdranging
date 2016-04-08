/*
 * bladeRF based ranging system implementation
 *
 * Stefan Biereigel
 */
#include <stdio.h>
#include <thread>
#include <libbladeRF.h>
#include <signal.h>
#include "bladerf.h"
#include "ranging_rx.h"
#include "ranging_tx.h"

extern void rx_thread(struct bladerf *dev);
extern void tx_thread(struct bladerf *dev);

int volatile done = 0;
int volatile calibration_running = 1;

void intHandler(int signal) {
	if (calibration_running) {
		calibration_running = 0;
		printf("Finishing calibration..\n");
	} else {
		done = 1;
		printf("Exiting..\n");
	}
}

int main(int argc, char *argv[])
{
	int status;
	struct module_config config;

	struct bladerf *dev = NULL;
	struct bladerf_devinfo dev_info;

	bladerf_init_devinfo(&dev_info);
	
	std::thread h_rx_thread;
	std::thread h_tx_thread;

	status = bladerf_open_with_devinfo(&dev, &dev_info);
	if (status != 0) {
		fprintf(stderr, "Unable to open device: %s\n",
				bladerf_strerror(status));

		return 1;
	}
	
	/* Set up RX module parameters */
	config.module	  = BLADERF_MODULE_RX;
	config.frequency  = 433000000;
	config.bandwidth  = 1500000;
	config.samplerate = 2000000;
	config.rx_lna	  = BLADERF_LNA_GAIN_MID;
	config.vga1		  = 30;
	config.vga2		  = 0;

	status = configure_module(dev, &config);
	if (status != 0) {
		fprintf(stderr, "Failed to configure RX module. Exiting.\n");
		goto out;
	}

	/* Set up TX module parameters */
	config.module	  = BLADERF_MODULE_TX;
	config.frequency  = 433000000;
	config.bandwidth  = 1500000;
	config.samplerate = 2000000;
	config.vga1		  = -20;
	config.vga2		  = 0;

	status = configure_module(dev, &config);
	if (status != 0) {
		fprintf(stderr, "Failed to configure TX module. Exiting.\n");
		goto out;
	}
	
	status = init_sync(dev);
	if (status != 0) {
		goto out;
	}
	
	status = bladerf_enable_module(dev, BLADERF_MODULE_RX, true);
	if (status != 0) {
		fprintf(stderr, "Failed to enable RX module: %s\n",
				bladerf_strerror(status));
		goto out;
	}

	status = bladerf_enable_module(dev, BLADERF_MODULE_TX, true);
	if (status != 0) {
		fprintf(stderr, "Failed to enable RX module: %s\n",
				bladerf_strerror(status));
		goto out;
	}

	signal(SIGINT, intHandler);
	
	/* start RX thread with device handle */
	h_rx_thread = std::thread(rx_thread, dev);
	h_tx_thread = std::thread(tx_thread, dev);

	h_rx_thread.join();
	h_tx_thread.join();

out:
	/* Disable RX module, shutting down our underlying RX stream */
	status = bladerf_enable_module(dev, BLADERF_MODULE_RX, false);
	if (status != 0) {
		fprintf(stderr, "Failed to disable RX module: %s\n",
				bladerf_strerror(status));
	}

	/* Disable TX module, shutting down our underlying TX stream */
	status = bladerf_enable_module(dev, BLADERF_MODULE_TX, false);
	if (status != 0) {
		fprintf(stderr, "Failed to disable TX module: %s\n",
				bladerf_strerror(status));
	}

	bladerf_close(dev);
	return status;
}

