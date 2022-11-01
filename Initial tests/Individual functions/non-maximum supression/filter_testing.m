clear;
I = imread('testCleanPlate.jpg');
%I = flip(flip(I, 1), 2);
I_edge = sobel_filter(gauss_blur(I, 19));

imwrite(I_edge(:,:,1), 'out/preSupressed.png');

I_sup = non_max_supression(I_edge);

imwrite(I_sup, 'out/supressed.png');

threshold = 55;

thres_filtered = zeros( size(I_sup) );
for i = 1:size(thres_filtered, 2)
    for j = 1:size(thres_filtered,1)
        if( I_sup(j,i) >= threshold )
            thres_filtered(j,i) = I_sup(j,i);
        end
    end
end

imwrite(thres_filtered, 'out/thresFiltered.png');