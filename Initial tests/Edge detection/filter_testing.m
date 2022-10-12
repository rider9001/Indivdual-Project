clear;
I = imread('testCleanPlate.jpg');
%I = flip(flip(I, 1), 2);
I_edge = sobel_filter(I, 170);

stats = [regionprops(I_edge); regionprops(not(I_edge))];

imshow(I_edge);
% hold on;
% for i = 1:numel(stats)
%     rectangle('Position', stats(i).BoundingBox, ...
%     'Linewidth', 1, 'EdgeColor', 'r', 'LineStyle', '--');
% end

imwrite(I_edge, 'out.jpg');