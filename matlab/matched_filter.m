% convolution of different filter waveforms

len = 100;

seq = (rand(1, 20) > 0.5) * 2 - 1;
rect = ones(1, len);
rect_transmit = filter(rect, 1, upsample(seq, len));
rect = rect / sum(rect);
rect_out = filter(rect, 1, rect_transmit);

subplot(3,1,1)
plot(rect_out);

rc = rcosine(1, len);
rc_transmit = filter(rc, 1, upsample(seq, len));
rc = rc / sum(rc);
rc_out = filter(rc, 1, rc_transmit);

subplot(3,1,2)
plot(rc_transmit);

hs = sin(pi*(0:(len-1))/len);
hs_transmit = filter(hs, 1, upsample(seq, len));
hs = hs / sum(hs);
hs_out = filter(hs, 1, hs_transmit);

subplot(3,1,3)
plot(hs_out);

