function outImg = gauss_blur(inpImg, kernalLen)

%kernel can only ever be odd to ensure a centre number
if mod(kernalLen, 2) ~= 1
    kernalLen = kernalLen + 1;
end
inpImg = double(inpImg);
binomCoeffs = [];

% the binomial list is wieghted to exculde the small values as compared to
% other wieghts they make their pixels near wieghtless, effectively
% shrinking the kernel
for i = 1:kernalLen + 1
    binomCoeffs = [binomCoeffs (nchoosek(kernalLen + 2, i))];
end

finalDivider = sum(binomCoeffs)^2;

outImg = zeros(size(inpImg));

for i = ceil(kernalLen / 2) : size(inpImg, 1) - ceil(kernalLen / 2)
    for j = ceil(kernalLen / 2) : size(inpImg, 2) - ceil(kernalLen / 2)
        %get frame of kernel, for 9x9 this would aim to get -4 to 4 from
        %the origin i,j in both x and y direction
        frame = inpImg( i-floor(kernalLen / 2):i+floor(kernalLen / 2) , j-floor(kernalLen / 2):j+floor(kernalLen / 2));

        for k = 1:size(frame,1)
            for l = 1:size(frame,1)
                frame(k,l) = (frame(k,l) * binomCoeffs(k) * binomCoeffs(l)) / finalDivider;
            end
        end

        outImg(i,j) = sum(sum(frame));

    end
end

outImg = uint8(outImg);

end