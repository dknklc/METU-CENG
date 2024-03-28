function [L,U] = shermans(A)
    [n,~] = size(A);
    if n == 1            % base case when the size of the matrix is 1
        L(1,1) = eye(1);
        U(1,1) = A(1,1);
    elseif n == 2        % base case when the size of the matrix is 2x2
        L = eye(2);      % return a 2x2 identity matrix
        U = zeros(2);    % return a 2x2 matrix consisting of all zeros
        L(2,1) = A(2,1) / A(1,1);
        U(1,1) = A(1,1);
        U(1,2) = A(1,2);
        U(2,2) = A(2,2) - L(2,1)*U(1,2);
    else
        [lower,upper] = shermans(A(1:size(A,1) - 1 , 1:size(A,1) - 1));
        L = eye(size(A,1));
        U = zeros(size(A,1));
        [r,~] = size(A);
        L(1:r - 1 , 1:r - 1) = lower;
        U(1:r - 1 , 1:r - 1) = upper;
        U(1:r-1, r) = lower \ A(1:r-1 , r);
        L(r , 1:r - 1) = A(r, 1:r - 1) / upper;
        U(r , r) = A(r, r) - L(r , 1:r - 1)*U(1:r-1, r);
    end
end
