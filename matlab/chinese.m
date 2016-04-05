% Chinese Reminder Theorem, matlabworld-implementation.

clear variables;
close all;
clc;

modulos = [5 7 9]; % the bases should be relativly prime
max_ind = [3 5 7]; % the number represention (g) encryted in a unique way


% Objective: To find the smallest x such that: 
% a. g=mod(x,b) or written in another way
% b. x =(%b) g  

[bx by] = meshgrid(modulos, modulos);
bb = gcd(bx,by)-diag(modulos);
pp = ~sum(sum(bb>1)); 

if (pp)
    display(['The Bases [relativly prime] are: b=[' num2str(modulos) ']'])
    display(['The Number [representation] is : g=<' num2str(max_ind) '>' ])
    
    % take out one by one bases and replace with 1's 
    xo = by-diag(modulos-1);
    
    % and get the product of the others
    Mk = prod(xo);
    
    % now we should get an solution for x and xa where Mk.*xa =(%b) x =(%b) 1
    % note that xa.*g is a solution, i.e xa.*g =(%b) g, because xa =(%b) ones
    [Gk, nk, Nk] = gcd ( modulos, Mk ) ;
    % [G,C,D] = GCD( A, B ) also returns C and D so that G = A.*C + B.*D.
    % These are useful for solving Diophantine equations and computing
    % Hermite transformations.
    
    % Then the strange step
    Sum_g_Nk_Mk = sum ( (max_ind .* Nk) .* Mk ) ;
    
    % get the lowest period unique answer between [0:(product(b)-1)]
    x = mod(Sum_g_Nk_Mk,prod(modulos));
    
    display(['The Number [lowest unique value] is: x=''' num2str(x) '''' ])
else
    display('The Bases are NOT Relprime.')
end
