function [maxEigenvalue,maxEigenvector] = power_method(A,v)
    vPrev = v;
    err = 10^-4;
    for i = 1:10000
        v = A*v;
        v = v / norm(v);
        if norm(v - vPrev) < err
            break
        end
        vPrev = v;
    end
    maxEigenvalue = (v.' * (A*v)) / (v.' * v);        % Rayleigh Quatient
    maxEigenvector = v;
end
