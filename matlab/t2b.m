% Balanced Weighted-voting T = 2 Tausworthe code generation

clear all

L = 2*7*11*15*19*23;
code = zeros(1, L);

c{1} = [+1 -1];
c{2} = [+1 +1 +1 -1 -1 +1 -1];
c{3} = [+1 +1 +1 -1 -1 -1 +1 -1 +1 +1 -1];
c{4} = [+1 +1 +1 +1 -1 -1 -1 +1 -1 -1 +1 +1 -1 +1 -1];
c{5} = [+1 +1 +1 +1 -1 +1 -1 +1 -1 -1 -1 -1 +1 +1 -1 +1 +1 -1 -1];
c{6} = [+1 +1 +1 +1 +1 -1 +1 -1 +1 +1 -1 -1 +1 +1 -1 -1 +1 -1 +1 -1 -1 -1 -1];

c_seq{1} = zeros(2, 2);
c_seq{2} = zeros(7, 7);
c_seq{3} = zeros(11, 11);
c_seq{4} = zeros(15, 15);
c_seq{5} = zeros(19, 19);
c_seq{6} = zeros(23, 23);

for i=1:6
    for j=1:length(c{i})
        c_seq{i}(j,:) = circshift(c{i}, -(j-1), 2);
    end
end

for i=1:L
    code(i) = sign( ...
        2 * c{1}(mod(i-1, 2)+1) + ...
        c{2}(mod(i-1, 7)+1) - ...
        c{3}(mod(i-1, 11)+1) - ...
        c{4}(mod(i-1, 15)+1) + ...
        c{5}(mod(i-1, 19)+1) - ...
        c{6}(mod(i-1, 23)+1));
end

c_out{1} = zeros(2, 1);
c_out{2} = zeros(7, 1);
c_out{3} = zeros(11, 1);
c_out{4} = zeros(15, 1);
c_out{5} = zeros(19, 1);
c_out{6} = zeros(23, 1);

i = 1;
code_start = 30000;
tcor = 10000;
for code_pos=code_start:(code_start+tcor)
    for j=1:6
        for k=1:length(c_seq{j})
            c_out{j}(k) = c_out{j}(k) + code(code_pos) * c_seq{j}(k, mod(i-1, length(c_seq{j}))+1);
        end
    end
    i = i + 1;
end

max_ind(1) = find(c_out{1} == max(c_out{1}));
max_ind(2) = find(c_out{2} == max(c_out{2}));
max_ind(3) = find(c_out{3} == min(c_out{3}));
max_ind(4) = find(c_out{4} == min(c_out{4}));
max_ind(5) = find(c_out{5} == max(c_out{5}));
max_ind(6) = find(c_out{6} == min(c_out{6}));

% chinese remainder theorem search for position in code
modulos = [2 7 11 15 19 23];

[bx, by] = meshgrid(modulos, modulos);
xo = by-diag(modulos-1);
Mk = prod(xo);
[Gk, nk, Nk] = gcd (modulos, Mk);
Sum_g_Nk_Mk = sum((max_ind .* Nk) .* Mk);
pos = mod(Sum_g_Nk_Mk, prod(modulos));


        