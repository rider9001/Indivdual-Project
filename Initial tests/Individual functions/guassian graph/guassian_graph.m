clear;
range = 10;
x = -range:0.1:range;
y = -range:0.1:range;

scaler = 1;

sigma = 3;

guassian = @(x,y) ( scaler/(2*pi*sigma^2) ) * exp( -(x.^2 + y.^2)/(2*sigma^2) );

fsurf(guassian, [-range range]);