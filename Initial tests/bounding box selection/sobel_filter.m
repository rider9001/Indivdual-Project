function filteredImg = sobel_filter(inputImg)
%returns a 3d matrix with the sobel convolution and direction for that
%pixel

%filter masks for vertical and horizontal images
s_filter_hor = [-1 0 1; -2 0 2; -1 0 1];
s_filter_ver = [-1 -2 -1; 0 0 0; 1 2 1];

%greyscale imput image
inputImg = im2gray(inputImg);
inputImg = double(inputImg);

%create output image
filteredImg = zeros( [size(inputImg) 2] );


% Edge Detection Process
% When i = 1 and j = 1, then filtered_image pixel  
% position will be filtered_image(2, 2)
% The mask is of 3x3, so we need to traverse 
% to filtered_image(size(input_image, 1) - 2
%, size(input_image, 2) - 2)
% Thus we are not considering the borders.
for i = 1:size(inputImg, 1) - size(s_filter_hor, 1) - 1
    for j = 1:size(inputImg, 2) - size(s_filter_hor, 1) - 1
  
        % Gradient approximations
        Gx = sum(sum(s_filter_hor.*inputImg(i:i+size(s_filter_hor, 1) - 1, j:j+size(s_filter_hor, 1) - 1)));
        Gy = sum(sum(s_filter_ver.*inputImg(i:i+size(s_filter_hor, 1) - 1, j:j+size(s_filter_hor, 1) - 1)));
                 
        % Calculate magnitude of vector, 
        % this convolutes the 2 value's magnitude
        tst = sqrt(Gx.^2 + Gy.^2);
%         if (tst >  255)
%             fprintf(sprintf("Tst = %d\n", tst))
%         end
        filteredImg(i+1, j+1, 1) = tst;

        %implementing the direction checking, can skip the atan as there
        %are only 4 unqiue directions, so basic comparison can find the
        %correct one
        %techincally Gy = Gx = 0 is an unchchecked case, but as this would
        %be a 0 edge wieght pixel, there isnt any nearby data being lost
        if(Gy == 0)
            %direction is up-down
            filteredImg(i+1, j+1, 2) = 0;
        elseif (Gx == 0)
            %direction is left-right
            filteredImg(i+1, j+1, 2) = 1;
        elseif (Gx * Gy > 0)
            %this is a same sign comparison
            %direction is upRight-downLeft
            filteredImg(i+1, j+1, 2) = 2;
        else
            %this state should only be reached if:
            %Gy && Gx > 0  and Gy/Gx is negative
            %direction is upLeft-downRight
            filteredImg(i+1, j+1, 2) = 3;
        end
    end
end

% convert back to integers for greyscale values
% can create values above 255 but i belive this is either scaled or clipped
filteredImg = uint8(filteredImg);

end

