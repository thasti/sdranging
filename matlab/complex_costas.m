% complex costas loop
clear all

% simulation setup
fs = 1e6;
dt = 1/fs;
fsym = 10e3;
samp_per_sym = fs/fsym;
f_sig = 10e3;
f_nco = 2*pi*0/fs;
n_bit = 1e4;

% calculate active PI loop filter
wn                = 2*pi*(fsym/30)/fs;
zeta              = 0.707;
K                 = 1000;

t1 = K/(wn*wn);
t2 = 2*zeta/wn;

b0 = (4*K/t1)*(1.+t2/2.0);
b1 = (8*K/t1);
b2 = (4*K/t1)*(1.-t2/2.0);

a1 = -2.0;
a2 =  1.0;

v0 = 0; v1 = 0; v2 = 0;

% prepare transmitted data and transmission signal
h = sin(pi*(0:samp_per_sym)/samp_per_sym);
h = h / sum(h);
d = ((rand(1, n_bit) > 0.5) * 2) - 1;
d = kron(d, ones(1, samp_per_sym));
t = 0:dt:n_bit/fsym-dt;
phase = d*pi/2;
bb_in = cos(phase) + 1j*sin(phase);
bb_in = filter(h, 1, real(bb_in)) + 1j*filter(h, 1, imag(bb_in));
doppler_lo = cos(2*pi*f_sig*t) + 1j*sin(2*pi*f_sig*t);
bb_in = bb_in .* doppler_lo;

post_filter = 0;
phase_error = 0;
phase_error_t = zeros(1,length(bb_in));
out_t = zeros(1,length(bb_in));

% costas loop
for i = 1:length(bb_in)
    nco_out = cos(phase_error) + 1j*sin(phase_error);
    
    post_mix = bb_in(i) * nco_out;
    
    pre_filter = real(post_mix) * imag(post_mix);

    v2 = v1;
    v1 = v0;
    v0 = pre_filter - v1*a1 - v2*a2;

    phase_error = v0*b0 + v1*b1 + v2*b2;
    
    phase_error_t(i) = phase_error;
    out_t(i) = imag(post_mix);
end

plot(out_t)