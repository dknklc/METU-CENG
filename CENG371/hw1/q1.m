answer = zeros(1,1000);
fs = zeros(1,1000);
for n = 1:1000
    f = n * (((n + 1) / n) - 1) - 1;
    fs(n) = f;
    g = f / eps;
    answer(n) = g;
end
n = 1:1:1000;

% -----  PLOTTING -----
plot(n,answer);
title("Plot of g(n)",'FontSize',16,'FontWeight','bold','Color','r');
ylabel("g(n) = f(n) / eps",'FontSize',12,'FontWeight','bold','Color','b');
xlabel("1 \leq n \leq 1000",'FontSize',12,'FontWeight','bold','Color','b');

% ------ PRINTING THE VALUES OF N AND G(N) THAT ARE EQUAL TO 0.
for i = 1:1000
    if answer(i) == 0
        fprintf(' n = %.30f g = %.30f\n',i, answer(i));
    end
end


%fprintf('eps = %.16f\n', eps(6));


