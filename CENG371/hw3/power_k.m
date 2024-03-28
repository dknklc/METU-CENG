function [eigValues,eigVectors] = power_k(A,k)
    n = size(A,1);
    eigValues = 1:k;
    eigVectors = zeros(n,k);
    err = 10^-4;
    for l = 1:k    
        x0 = 1:n;                           % arbitrary initial x0 vector
        x0 = x0 / norm(x0);
        x0 = x0.';
        vPrev = x0;
        for i = 1:10000
            x0 = A*x0;
            x0 = x0 / norm(x0);
            if norm(x0 - vPrev) < err
                break
            end
            vPrev = x0;
        end
        eigValues(l) = (x0.' * (A*x0)) / (x0.' * x0);        % Rayleigh Quatient
        eigVectors(:,l) = x0;
        A = A - eigValues(l)*eigVectors(:,l)*eigVectors(:,l).';
    end
end






























