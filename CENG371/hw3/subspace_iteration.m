function [eigValues, eigVectors] = subspace_iteration(A,k)
    n = size(A,1);
    V = rand(n,k);
    [Q,R] = qr(V);
    vPrev = Q(:,1:k);
    for i = 1:100000
        Q = A * Q;
        [Q,R] = qr(Q);
        if norm(Q(:,1:k) - vPrev) < 0.0001
            break;
        end
        vPrev = Q(:,1:k);
    end
    eigVectors = Q(:,1:k);
    eigValues = eigVectors.' * A * eigVectors;
    eigValues = diag(eigValues);
end









%{
function [eigValues, eigVectors] = subspace_iteration(A,k)
    [Q,R] = qr(A);
    for i = 1:1000
           Q = A * Q;
           for l = 1:k
                Q(:,l) = Q(:,l) / norm(Q(:,l));
                
           end
           [Q,R] = qr(Q);
    end
    eigVectors = Q(:,1:l);
    eigValues = (Q(:,1:l).' * (A*Q(:,1:l))) / (Q(:,1:l).' * Q(:,1:l));
    eigValues = diag(eigValues);
end
%}