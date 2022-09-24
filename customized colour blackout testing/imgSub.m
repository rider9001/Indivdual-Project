clear;

img1 = flip(flip(imread("IMG1.jpg"),1), 2);
img2 = flip(flip(imread("IMG2.jpg"),1), 2);
img1 = im2double(img1);
img2 = im2double(img2);

filterRange = [40 55];

fil1 = colourfilter(img1, filterRange);
fil2 = colourfilter(img2, filterRange);

out1 = imsubtract(imcomplement(fil1), fil2);
out2 = imsubtract(imcomplement(fil2), fil1);

imwrite(fil1, "fil1.jpg");
imwrite(fil2, "fil2.jpg");

imwrite(out1, "out1.jpg");
imwrite(out2, "out2.jpg");