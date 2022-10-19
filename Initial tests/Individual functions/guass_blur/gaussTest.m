clear;
inpImg = double(im2gray(imread('testCleanPlate.jpg')));

%can only ever be odd to ensure a centre number
kernalLen = 9;

binomCoeffs = [];

for i = 1:kernalLen
    binomCoeffs = [binomCoeffs (nchoosek(kernalLen, i))];
end

%finalDivider = sum(binomCoeffs);
binomCoeffs = binomCoeffs ./ sum(binomCoeffs);

I_out = zeros(size(inpImg));

for i = ceil(kernalLen / 2) : size(inpImg, 1) - ceil(kernalLen / 2)
    for j = ceil(kernalLen / 2) : size(inpImg, 1) - ceil(kernalLen / 2)
        %get frame of kernel, for 9x9 this would aim to get -4 to 4 from
        %the origin i,j in both x and y direction
        frame = inpImg( i-floor(kernalLen / 2):i+floor(kernalLen / 2) , j-floor(kernalLen / 2):j+floor(kernalLen / 2));

        for k = 1:size(frame,1)
            % L = L .* v(:, ones(N,1));
            frame = binomCoeffs .* frame(:, k);
            frame = transpose(binomCoeffs) .* frame(k, :);
        end

        I_out(i,j) = sum(sum(frame));

    end
end

%I_out = imgaussfilt(inpImg, 7);

I_out = uint8(I_out);
imshow(I_out);