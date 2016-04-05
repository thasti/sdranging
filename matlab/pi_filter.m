% PI filter test

clear all

% calculate active PI loop filter
wn                = 10;
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

N = 1000;
out_t = zeros(1, N);
in = 10;
for i = 1:N
    v2 = v1;
    v1 = v0;
    v0 = in - v1*a1 - v2*a2;

    out_t(i) = v0*b0 + v1*b1 + v2*b2; 
    
end

plot(diff(diff(out_t)))