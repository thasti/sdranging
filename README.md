# Software-defined Ranging transmitter / receiver

The code in this repository implements software components used in ranging, for exmaple in satellite applications. The implementation is loosely based on the recommendations in CCSDS 414.1-B-2. It is implemented for the bladeRF SDR, which is able to do full-duplex, fully synchronous Rx/TX operation. 

## Operation / Design
After starting the application, the ranging signal is generated on the transmitter port, and the receiver is started. The TX port is internally connected via loopback to the RX port for calibration. As soon as the receiver carrier & timing syncronizers lock to the transmitted PN-sequence, the calibration process will begin to determine the sample offset between transmitter and receiver. When the offset has settled, Ctrl-C finishes the calibration process and the measurement can begin. 
When the TX and RX ports are connected to their corresponding amplifiers / antennas. After sequence syncronisation, the calculated two-way range to the target is continually output. The software can finally be stopped by hitting Ctrl-C.

## Tradeoffs
Amateur radio transponders, whether ground-based or on the OSCARs do not feature wide bandwidths like the deep-space missions, therefore using very high chip rates (and achieving meter-scale-resolutions) is not possible. As a tradeoff, a relatively low chip rate of 10 kHz with bandwidth-efficient sine wave pulse shaping is implemented, and oversampled to acheive a resolution of ~300m of two-way range. 
