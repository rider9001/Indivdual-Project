function binImgOut = edge_link(inpImg, threshold, strong_pixel_needed, adjancency_limit)

binImgOut = false(size(inpImg));
%adjacency defines the number of pixels around the current pixel that will
%be considered when calculating the strength of the current pixel
% 1 would be a radius of 1 pixel around the current
% value must be odd
adjancency_limit = adjancency_limit + 2;

%j is y axis, i is x axis
for j = ceil(adjancency_limit / 2) : size(inpImg, 2) - ceil(adjancency_limit / 2)
    for i = ceil(adjancency_limit / 2) : size(inpImg, 1) - ceil(adjancency_limit / 2)
        if(inpImg(i,j) >= threshold)
            binImgOut(i,j) = true;
        else
            frame = inpImg( i-floor(adjancency_limit / 2):i+floor(adjancency_limit / 2) , j-floor(adjancency_limit / 2):j+floor(adjancency_limit / 2));
            
            strongPixCount = 0;
            for k = 1:size(frame, 2)
                for l = 1:size(frame, 1)
                    if (frame(k,l) >= threshold)
                        strongPixCount = strongPixCount + 1;
                    end
                end
            end

            if(strongPixCount >= strong_pixel_needed)
                binImgOut(i,j) = true;
            else
                binImgOut(i,j) = false;
            end
        end
    end
end

end