clear;
I = imread('plateFar/plateFar1080p.png');
%I = flip(flip(I, 1), 2);
I_edge = imbinarize(sobel_filter(I, 210));

imshow(I_edge);

%find both black and white regions
stats = [regionprops(I_edge); regionprops(not(I_edge))];
hold on;
for i = 1:numel(stats)
    rectangle('Position', stats(i).BoundingBox, ...
    'Linewidth', 1, 'EdgeColor', 'r', 'LineStyle', '--');
end

imwrite(I_edge, 'out.jpg');