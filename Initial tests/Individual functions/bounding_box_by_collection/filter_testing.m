clear;
delete('out/*');

I = imread('plateDay1m.jpg');
%I = flip(flip(I, 1), 2);

gauss_kernel_len = 9;

fprintf('Gaussian blur time:\n');
tic;
I_gauss = gauss_blur(I, gauss_kernel_len);
toc;
fprintf('----------------------\n');

fprintf('Sobel filter time:\n');
tic;
I_edge = sobel_filter(I_gauss);
toc;
fprintf('----------------------\n');

imwrite(I_edge(:,:,1), 'out/SobelMag.png');

fprintf('Non-max supression time:\n');
tic;
I_sup = non_max_supression(I_edge);
toc;
fprintf('----------------------\n');

imwrite(I_sup, 'out/supressed.png');

scalingFactor = 4;
fprintf('Scaling time:\n');
tic;
I_sup = I_sup(:,:) * scalingFactor;
toc;
fprintf('----------------------\n');

imwrite(I_sup, 'out/scaled.png');

threshold = 100;
adj_strongs_req = 3;
%adjacency MUST be odd 
linking_adjacency = 1;

fprintf('Edge linking time:\n');
tic;
outBinImg = edge_link(I_sup, threshold, adj_strongs_req, linking_adjacency);
toc;
fprintf('----------------------\n');

imwrite(outBinImg, 'out/edgeLinked.png')
imshow(outBinImg);

bounding_adjacency = 3;

fprintf('Bounding time:\n');
tic;
boxCoords = shape_bound_box_finder(outBinImg, bounding_adjacency);
toc;
fprintf('----------------------\n');
%charming syntax from MATLAB here


for idx = 1:numel(boxCoords)
    currBox = [boxCoords(idx).x1, boxCoords(idx).y1, (boxCoords(idx).x2 - boxCoords(idx).x1), (boxCoords(idx).y2 - boxCoords(idx).y1)];
    rectangle('Position', currBox, ...
     'LineWidth', 1, 'EdgeColor', 'r', 'LineStyle', '--');
end

% %find both black and white regions
% stats = regionprops(out, 'BoundingBox');
% hold on;
% for j = 1:numel(stats)
%     rectangle('Position', stats(j).BoundingBox, ...
%     'LineWidth', 1, 'EdgeColor', 'r', 'LineStyle', '--');
% end

% for i = 1:9
%     edge_linked = edge_link(I_sup, threshold, i, adjacency);
%     imwrite(edge_linked, sprintf('out/thresFiltered%i.png', i));
% end