clear;
I = imread('plateFar/plateFar1080p.png');
%I = flip(flip(I, 1), 2);
I_edge = imbinarize(sobel_filter(I, 210));

I_edge = bwareafilt(I_edge, [100, 1000]);

% find both black and white regions
% stats = [regionprops(I_edge); regionprops(not(I_edge))];
% hold on;
% for i = 1:numel(stats)
%     rectangle('Position', stats(i).BoundingBox, ...
%     'Linewidth', 1, 'EdgeColor', 'r', 'LineStyle', '--');
% end

imshow(I_edge);

imwrite(I_edge, 'out.jpg');