function [L, U] = picketts(A)
    [r,c] = size(A);
    if c == 1
        L(1,1) = eye(1);
        U(1,1) = A(1,1);
    elseif c == 2
        L = eye(r,2);
        U = zeros(r,2);
        L(2,1) = A(2,1) / A(1,1);
        U(1,1) = A(1,1);
        U(1,2) = A(1,2);
        U(2,2) = A(2,2) - L(2,1)*U(1,2);
        L(2:r, 1:c) = A(2:r, 1:c) / U(1:2,1:2);
    else
        % check below
        [n,k] = size(A);
        [lower,upper] = picketts(A(1:size(A,1) , 1:size(A,2) - 1));
        L = eye(size(A,1),size(A,2));
        U = zeros(size(A,1),size(A,2));
        L(1:size(A,1) , 1:size(A,2) - 1) = lower;
        U(1:size(A,1) , 1:size(A,2) - 1) = upper;
        U(1:size(A,2) - 1 , k) = lower(1:size(A,2) - 1 , 1:k-1) \ A(1:size(A,2) - 1 ,size(A,2));
        L(size(A,2):size(A,1) , size(A,2)) = A(size(A,2):size(A,1) , size(A,2)) - L(size(A,2):size(A,1) , 1:size(A,2)-1) * U(1:size(A,2)-1 , size(A,2));
        U(size(A,2),size(A,2)) = L(size(A,2),size(A,2));
        L(size(A,2):size(A,1), size(A,2)) = L(size(A,2):size(A,1) , size(A,2)) / U(size(A,2),size(A,2));       
    end
end