
RelErrShermans = zeros(1,300);
TimeShermans = zeros(1,300);
RelErrPicketts = zeros(1,300);
TimePicketts = zeros(1,300);
for n = 1:300
    A = hilb(n);
    tic
    [L,U] = shermans(A);
    TimeShermans(n) = toc;
    RelErrShermans(n) = norm(A - L*U) / norm(A);
end
for n = 1:300
    A = hilb(n);
    tic
    [L,U] = picketts(A);
    TimePicketts(n) = toc;
    RelErrPicketts(n) = norm(A - L*U) / norm(A);
end

n = 1:300;
semilogy(n, RelErrShermans, n, RelErrPicketts);
title("Relative Errors of LU Factorizations",'FontSize',16,'FontWeight','bold','Color','r');
ylabel("Relative Error",'FontSize',12,'FontWeight','bold','Color','b');
xlabel("1 \leq n \leq 300",'FontSize',12,'FontWeight','bold','Color','b');
legend("Sherman's march", "Pickett's charge");


RunTimeOfShermans = sum(TimeShermans)
RunTimeOfPicketts = sum(TimePicketts);

fprintf("Run Time of Sherman's March = %.16f sec \n",RunTimeOfShermans);
fprintf("Run Time of Pickett's Charge = %.16f sec \n",RunTimeOfPicketts);
