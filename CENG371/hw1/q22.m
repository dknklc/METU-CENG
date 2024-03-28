nums = zeros(1,10^6);
for n = 1:10^6
    nums(n) = 1 + (10^6 + 1 - n)*10^(-8);
end

numss = single(zeros(1,10^6));
for n = 1:10^6
    numss(n) = 1 + (10^6 + 1 - n)*10^(-8);
end

compensatedDouble = @() compansatedSumDouble(nums);
timeCompensatedDoube = timeit(compensatedDouble);
resultOfCompensatedDouble = (compensatedDouble());
compensatedSingle = @() compansatedSumSingle(numss);
timeCompensatedSingle = timeit(compensatedSingle);
resultOfCompensatedSingle = (compensatedSingle());

fprintf('Compensated Double Precision : %.16f\n', resultOfCompensatedDouble);
fprintf('Compensated Single Precision : %.16f\n', resultOfCompensatedSingle);


%compensated summation in double precision
function sum = compansatedSumDouble(nums)
    sum = 0.0;
    e = 0.0;

    for i=1:length(nums)
        y = nums(i) - e;
        t = sum + y;
        e = (t - sum) - y;
        sum = t;
    end
end


%compensated summation in single precision
function sum = compansatedSumSingle(numss)
    sum = single(0.0);
    e = single(0.0);

    for i=1:length(numss)
        
        y = numss(i) - e;
        t = sum + y;
        e = (t - sum) - y;
        sum = t;
    end
end




