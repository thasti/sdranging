% chip tracking loop
% run t2b.m beforehand

clearvars -except code
close all

osf = 20;
code_chips = 1e3;
code_osf = kron(code(1:code_chips), ones(1, osf));
code_osf = filter(ones(1, osf), 1, code_osf)/osf;

wn                = 2*pi*(1/20)/osf;
zeta              = 1;
K                 = (1/((1/pi)*osf));
% gain of TED is ~4 near 0
% gain of VCO is 1
% loop is run at 1/osf samples

g1 = (1-exp(-2*zeta*wn))/K;
g2 = (1+exp(-2*zeta*wn)-2*exp(-zeta*wn)*cos(wn*sqrt(1-zeta.^2)))/K;

osf_nom = 20;

phase_inc = 1/(osf_nom);
phase_acc = 0;

phase_acc_t = zeros(1, length(code_osf));
sampling_instant = zeros(1, length(code_osf));

e_t = 0;
integrator = 0;


for i=osf+8:length(code_osf)-osf
    if phase_acc >= 1
        e = (code_osf(i) - code_osf(i-osf_nom)) * code_osf(i-osf_nom/2);
        integrator = integrator + e;
        phase_acc = g1 * e + g2 * integrator;
        sampling_instant(i) = 1;
        e_t = [e_t e];
    end
    phase_acc = phase_acc + phase_inc;
    phase_acc_t(i) = phase_acc;
end

subplot(2,1,1)
stem(sampling_instant)
hold on
plot(code_osf)
subplot(2,1,2)
hold on
plot(e_t)
