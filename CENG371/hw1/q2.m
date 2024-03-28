nums = zeros(1,10^6);
for n = 1:10^6
    nums(n) = 1 + (10^6 + 1 - n)*10^(-8);
end

numss = single(zeros(1,10^6));
for n = 1:10^6
    numss(n) = 1 + (10^6 + 1 - n)*10^(-8);
end

naiveDouble = @() naiveSumDouble(nums);
timeNaiveDouble = timeit(naiveDouble);
resultOfNaiveDouble = (naiveDouble());
naiveSingle = @() naiveSumSingle(numss);
timeNaiveSingle = timeit(naiveSingle);
resultOfNaiveSingle = (naiveSingle());

fprintf('Naive Double Precision : %.16f\n',resultOfNaiveDouble);
fprintf('Naive Single Precision : %.16f\n',resultOfNaiveSingle);

function sum = naiveSumDouble(nums)
    sum = 0.0;
    for i = 1:10^6
        sum = sum + nums(i);
    end
end

function sum = naiveSumSingle(numss)
    sum = single(0.0);
    for i = 1:10^6
        sum = sum + numss(i);
    end
end




