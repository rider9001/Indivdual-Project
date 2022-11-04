clear;
delete('out/*');

I = imread('img2.jpg');
%I = flip(flip(I, 1), 2);
I_edge = sobel_filter(gauss_blur(I, 19));

imwrite(I_edge(:,:,1), 'out/preSupressed.png');

I_sup = non_max_supression(I_edge);

imwrite(I_sup, 'out/supressed.png');

scalingFactor = 4;
I_sup = I_sup(:,:) * scalingFactor;

imwrite(I_sup, 'out/scaled.png')

threshold = 125;
adjacency = 3;

out = edge_link(I_sup, threshold, 9, adjacency);
imshow(out);
%find both black and white regions
stats = [regionprops(out); regionprops(not(out))];
hold on;
for j = 1:numel(stats)
    rectangle('Position', stats(j).BoundingBox, ...
    'LineWidth', 1, 'EdgeColor', 'r', 'LineStyle', '--');
end

% for i = 1:9
%     edge_linked = edge_link(I_sup, threshold, i, adjacency);
%     imwrite(edge_linked, sprintf('out/thresFiltered%i.png', i));
% end