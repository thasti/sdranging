% complex costas loop
clear all

% simulation setup
fs = 2e5;
dt = 1/fs;
fsym = 10e3;
samp_per_sym = fs/fsym;
f_sig = 10e3;
n_bit = 1e4;

% calculate active PI loop filter
wn                = 2*pi*(fsym/100)/fs;
zeta              = 0.707;
K                 = (1/(2*pi*2*pi));

g1 = (1-exp(-2*zeta*wn))/K;
g2 = (1+exp(-2*zeta*wn)-2*exp(-zeta*wn)*cos(wn*sqrt(1-zeta.^2)))/K;

% prepare transmitted data and transmission signal
h = sin(pi*(0:samp_per_sym)/samp_per_sym);
h = h / sum(h);
d = ((rand(1, n_bit) > 0.5) * 2) - 1;
d = kron(d, ones(1, samp_per_sym));
t = 0:dt:n_bit/fsym-dt;
phase = d*pi/2;
bb_in = cos(phase) + 1j*sin(phase);
bb_in = filter(h, 1, real(bb_in)) + 1j*filter(h, 1, imag(bb_in));
%doppler_lo = cos(2*pi*f_sig*t) + 1j*sin(2*pi*f_sig*t);
doppler_lo = cos(2*pi*f_sig*(1:length(bb_in))/fs) + 1j*sin(2*pi*f_sig*(1:length(bb_in))/fs);
bb_in = bb_in .* doppler_lo;

integrator = 0;
phase_acc = pi;
phase_inc = 0;
phase_inc_t = zeros(1,length(bb_in));
out_t = zeros(1,length(bb_in));

% costas loop
for i = 1:length(bb_in)
    nco_out = cos(phase_acc) + 1j*sin(phase_acc);
    
    post_mix = bb_in(i) * nco_out;
    
    phase_detector = real(post_mix) * imag(post_mix);
    integrator = integrator + phase_detector;
    
    phase_inc = g1 * phase_detector + g2 * integrator;
    phase_acc = phase_acc + phase_inc;
    phase_acc = rem(phase_acc, 2*pi);
    
    phase_inc_t(i) = phase_inc;
    out_t(i) = imag(post_mix);
end

plot(out_t)