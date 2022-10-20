clear;
inpImg = double(im2gray(imread('testCleanPlate.jpg')));


% kernelSize = blurRuns * 2 - 1;
kernelSize = 15;
I_out = gauss_blur(inpImg, kernelSize);
% for i = kernelSize * 2 - 1:-2:3
%       I_out = gauss_blur(I_out, i);
% end

I_out = uint8(I_out);
I_out_edge = edge(I_out, 'sobel');

imwrite(I_out, 'blured.png');
imwrite(edge(inpImg, 'sobel'), 'baseEdge.png')
imwrite(I_out_edge, 'blurEdge.png');

imshow(I_out_edge);
%find both black and white regions
stats = [regionprops(I_out_edge); regionprops(not(I_out_edge))];
hold on;
for j = 1:numel(stats)
    rectangle('Position', stats(j).BoundingBox, ...
'LineWidth', 1, 'EdgeColor', 'r', 'LineStyle', '--');
end