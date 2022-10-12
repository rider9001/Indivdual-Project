clear;
I = imread('testCleanPlate.jpg');
%I = flip(flip(I, 1), 2);
I_edge = robert_filter(I, 120);

imshow(I_edge);
% hold on;
% for i = 1:numel(stats)
%     rectangle('Position', stats(i).BoundingBox, ...
%     'Linewidth', 1, 'EdgeColor', 'r', 'LineStyle', '--');
% end

imwrite(I_edge, 'out.jpg');