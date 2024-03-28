function [eigValue,eigVector] = inverse_power(A,alpha)
    n = size(A,1);
    x0 = 1:n;          % arbitrary initial x0 vector
    x0 = x0.';

    x0Prev = x0;
    err = 10^-4;
    for i = 1:10000      
        [L,U,P] = lu(A-alpha*eye(n));
        y = L\(P*x0);
        x0 = U\y;
        x0 = x0 / norm(x0);

        if norm(x0 - x0Prev) < err
            break
        end
    end
    eigValue = (x0.' * (A*x0)) / (x0.' * x0);        % Rayleigh Quatient
    eigVector = x0;
end