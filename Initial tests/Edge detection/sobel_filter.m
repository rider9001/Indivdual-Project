function filteredImg = sobel_filter(inputImg, threshold)
%sensitivity varies between [0 255]

%filter masks for vertical and horizontal images
s_filter_hor = [-1 0 1; -2 0 2; -1 0 1];
s_filter_ver = [-1 -2 -1; 0 0 0; 1 2 1];

%greyscale imput image
inputImg = im2gray(inputImg);
inputImg = double(inputImg);

%create output image
filteredImg = zeros( size(inputImg) );


% Edge Detection Process
% When i = 1 and j = 1, then filtered_image pixel  
% position will be filtered_image(2, 2)
% The mask is of 3x3, so we need to traverse 
% to filtered_image(size(input_image, 1) - 2
%, size(input_image, 2) - 2)
% Thus we are not considering the borders.
for i = 1:size(inputImg, 1) - 2
    for j = 1:size(inputImg, 2) - 2
  
        % Gradient approximations
        Gx = sum(sum(s_filter_hor.*inputImg(i:i+2, j:j+2)));
        Gy = sum(sum(s_filter_ver.*inputImg(i:i+2, j:j+2)));
                 
        % Calculate magnitude of vector, 
        % this convolutes the 2 value's magnitude
        filteredImg(i+1, j+1) = sqrt(Gx.^2 + Gy.^2);        
    end
end

% convert back to integers for greyscale values
% never seems to create values outside of 0-255, need to see why
filteredImg = uint8(filteredImg);

% for i = 1:size(filteredImg, 1)
%     for j = 1:size(filteredImg, 2)
%         if filteredImg(i,j) > threshold
%             filteredImg(i,j) = 255;
%         else
%             filteredImg(i,j) = 0;
%     end
% end

end

