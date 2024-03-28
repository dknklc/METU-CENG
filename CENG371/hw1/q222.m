nums = zeros(1,10^6);
for n = 1:10^6
    nums(n) = 1 + (10^6 + 1 - n)*10^(-8);
end

numss = single(zeros(1,10^6));
for n = 1:10^6
    numss(n) = 1 + (10^6 + 1 - n)*10^(-8);
end

pairwiseDouble =@() pairwiseSumDouble(nums);
timePairwiseDouble = timeit(pairwiseDouble);
pairwiseSingle = @() single(pairwiseSumSingle(numss));
timePairwiseSingle = timeit(pairwiseSingle);
fprintf('Pairwise Double Precision : %.16f\n',(pairwiseDouble()));
fprintf('Pairwise Single Precision : %.16f\n',(pairwiseSingle()));


% pairwise summation in double precision

function pws = pairwiseSumDouble(nums)
    if length(nums) == 1
       pws = nums;
    else
        m = floor(length(nums) / 2);
        pws = pairwiseSumDouble(nums(1,1:m)) + pairwiseSumDouble(nums(1,m+1:end));
    end
end

% pairwise summation in single precision

function pws = pairwiseSumSingle(numss)
    if length(numss) == 1
       pws = numss;
    else
        m = floor(length(numss) / 2);
        pws = pairwiseSumSingle(numss(1,1:m)) + pairwiseSumSingle(numss(1,m+1:end));
    end
end

