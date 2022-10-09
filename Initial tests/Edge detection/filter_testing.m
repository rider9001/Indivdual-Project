clear;
I = imread('coins.png');

I_edge = sobel_filter(I);

imshow(I_edge);