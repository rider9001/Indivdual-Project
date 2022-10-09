clear;
I = imread('testCleanPlate.jpg');
%I = flip(flip(I, 1), 2);
I_edge = sobel_filter(I, 190);

imshow(I_edge);
imwrite(I_edge, 'out.jpg');