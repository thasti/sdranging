% non-integrating second order butterworth filter

clear all

% calculate active PI loop filter
wn                = 1;
zeta              = 0.707;
K                 = (1/32.5);

g1 = (1-exp(-2*zeta*wn))/K;
g2 = (1+exp(-2*zeta*wn)-2*exp(-zeta*wn)*cos(wn*sqrt(1-zeta.^2)))/K;

N = 1000;
out_t = zeros(1, N);
in = 1;
integrator = 0;
out = 0;
for i = 1:N
    e = in;
    integrator = integrator + e;
    out = g1*e + g2*integrator;
    out_t(i) = out; 
end

plot(diff(out_t))