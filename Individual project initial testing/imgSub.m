img1 = rgb2gray(imread("IMG1.jpg"));
img2 = rgb2gray(imread("IMG2.jpg"));
back = rgb2gray(imread("background.jpg"));

out1 = imsubtract(img1, img2);
out2 = imsubtract(img2, img1);
bout1 = imsubtract(back, img1);
bout2 = imsubtract(back, img2);

imwrite(img1, "grey1.jpg");
imwrite(img2, "grey2.jpg");
imwrite(back, "backGrey.jpg");

imwrite(out1, "out1.jpg");
imwrite(out2, "out2.jpg");
imwrite(bout1, "backout1.jpg");
imwrite(bout2, "backout2.jpg");

%imshow(J2);