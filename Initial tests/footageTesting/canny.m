I = imread('img2.jpg');
I = flip(flip(I, 1), 2);
I = im2gray(I);

BW1 = edge(I,'sobel');
BW2 = edge(I,'canny');

tiledlayout(1,3);

nexttile
imshow(BW1)
imwrite(BW1, 'sobel.png');
title('Sobel Filter');

nexttile
imshow(BW2);
imwrite(BW2, 'canny.png');
title('Canny Filter');

nexttile
imshow(I);
title('Orignal');