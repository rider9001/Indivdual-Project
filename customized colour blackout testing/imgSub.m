clear;

img1 = flip(flip(imread("IMG1.jpg"),1), 2);
img2 = flip(flip(imread("IMG2.jpg"),1), 2);
img1 = im2double(img1);
img2 = im2double(img2);

filterRange = [45 55];

fil1 = colourfilter(img1, filterRange);
fil2 = colourfilter(img2, filterRange);

fil1gray = rgb2gray(fil1);
fil2gray = rgb2gray(fil2);

out1 = imsubtract(fil1gray, fil2gray);
out2 = imsubtract(fil2gray, fil1gray);

imwrite(fil1, "fil1.jpg");
imwrite(fil2, "fil2.jpg");

imwrite(fil1gray, "filgray1.jpg");
imwrite(fil2gray, "filgray2.jpg");

imwrite(out1, "out1.jpg");
imwrite(out2, "out2.jpg");