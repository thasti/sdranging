CC      = g++
CFLAGS  = -g -Wall -O3 -std=gnu++11
RM      = rm -f
LIBS	= -lm -lbladeRF -lpthread -lliquid
TARGETS = main.cc bladerf.cc ranging_rx.cc ranging_tx.cc costas.cc gardner.cc t2bgen.cc t2bcorr.cc

default: all

all: 
	$(CC) $(CFLAGS) -o bladerf_ranging $(TARGETS) $(LIBS)

clean:
	$(RM) bladerf_ranging
