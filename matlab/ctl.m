% chip tracking loop
% run t2b.m beforehand

clearvars -except code
close all

osf = 50;
code_chips = 1e3;
code_osf = kron(code(1:code_chips), ones(1, osf));
code_osf = filter(ones(1, osf), 1, code_osf)/osf;

lpf_k = 0.92;
lpf = 0;

osf_nom = 50;

phase_inc = 1/osf_nom;
phase_acc = 0;

phase_acc_t = zeros(1, length(code_osf));
sampling_instant = zeros(1, length(code_osf));

e_t = 0;
e_int = 0;
lpf_int = 0;


for i=osf+20:length(code_osf)-osf
    if phase_acc >= 1 
        e = (code_osf(i) - code_osf(i-osf_nom)) * code_osf(i-osf_nom/2);
        lpf = lpf_k * lpf + (1-lpf_k) * e;
        phase_acc = 0.45*lpf;
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
