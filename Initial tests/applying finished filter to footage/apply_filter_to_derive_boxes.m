function [bounding_boxes, binImgOut] = apply_filter_to_derive_boxes(inpImg, gauss_kernel_len, scalingFactor, threshold, adj_strongs_req, linking_adjacency, bounding_adjacency)

% fprintf('Gaussian blur time:\n');
% tic;
I_gauss = gauss_blur(inpImg, gauss_kernel_len);
% toc;
% fprintf('----------------------\n');

% fprintf('Sobel filter time:\n');
% tic;
I_edge = sobel_filter(I_gauss);
% toc;
% fprintf('----------------------\n');

% fprintf('Non-max supression time:\n');
% tic;
I_sup = non_max_supression(I_edge);
% toc;
% fprintf('----------------------\n');

% fprintf('Scaling time:\n');
% tic;
I_sup = I_sup(:,:) * scalingFactor;
% toc;
% fprintf('----------------------\n');

% fprintf('Edge linking time:\n');
% tic;
outBinImg = edge_link(I_sup, threshold, adj_strongs_req, linking_adjacency);
% toc;
% fprintf('----------------------\n');

% fprintf('Bounding time:\n');
% tic;
boxCoords = shape_bound_box_finder(outBinImg, bounding_adjacency);
% toc;
% fprintf('----------------------\n');
%charming syntax from MATLAB here

bounding_boxes = boxCoords;
binImgOut = outBinImg;

end