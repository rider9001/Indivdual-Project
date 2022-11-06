function pixelList = shape_pixel_finder(inpBinImg, adjancency_radius, startX, startY)
%returns a list where each struct contains the y, x coord of a pixel within the
%shape, list should contain all pixels of the shape

unCheckedPixels = struct('y',{},'x',{});
newPixel = struct('y',startY,'x', startX);
unCheckedPixels = [unCheckedPixels newPixel];
%the y, x format is only to conform with MATLAB, no specific preference or
%need in this algorithm

checkedPixels = struct('y',{},'x',{});

while(~isempty(unCheckedPixels))
    %pop next pixel from the unchecked list into current and checked list
    currentPixel = unCheckedPixels(1);
    checkedPixels = [checkedPixels currentPixel];
    unCheckedPixels(1) = [];

    %j is y axis, i is x axis
    for j = -floor(adjancency_radius/2):floor(adjancency_radius/2)
        for i = -floor(adjancency_radius/2):floor(adjancency_radius/2)

            %pixel search can go out of bounds, skip to next pixel if error
            %comes up
            try
                searchX = currentPixel.x + i;
                searchY = currentPixel.y + j;
                %if current search coord is postive
                if(inpBinImg(searchY, searchX))  

                    %add new found pixel to list to check for adjancent
                    %positives
                    newPixel = struct('y',searchY,'x',searchX);
                    unCheckedPixels = [unCheckedPixels newPixel];
                    %set the pixel low as it has been discovered
                    inpBinImg(searchY, searchX) = false;

                end
            catch except
                continue;
            end

        end
    end
end

pixelList = checkedPixels;

end