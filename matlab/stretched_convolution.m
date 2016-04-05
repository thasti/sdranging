% stretched convolution

clear all
close all

nbit = 1e3;
osf = 10;
rep = 3;
rand_seq = (rand(1, nbit) > 0.5) * 2 - 1;

rand_seq = kron(rand_seq, ones(1, osf));
input = kron(ones(1, rep), rand_seq);

input_stretched = resample(input, 1000, 999);
norm_conv = conv(input, fliplr(rand_seq));
stretched_conv = conv(input_stretched, fliplr(rand_seq));

figure
subplot(2,1,1);
plot(norm_conv)
subplot(2,1,2);
plot(stretched_conv)


