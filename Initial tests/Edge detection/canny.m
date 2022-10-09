I = imread('coins.png');
imshow(I);

BW1 = edge(I,'sobel');
BW2 = edge(I,'canny');

tiledlayout(1,3);

nexttile
imshow(BW1)
title('Sobel Filter');

nexttile
imshow(BW2);
title('Canny Filter');

nexttile
imshow(I);
title('Orignal');