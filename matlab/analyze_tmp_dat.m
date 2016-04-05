clear all

fd = fopen('../c/tmp.dat');
d = fread(fd, 'float');
d = d(1:2:end) + 1j*d(2:2:end);

plot(d);
min = min(min(real(d)), min(imag(d)));
max = max(max(real(d)), max(imag(d)));
axis([min max min max]);
axis square
fclose(fd);