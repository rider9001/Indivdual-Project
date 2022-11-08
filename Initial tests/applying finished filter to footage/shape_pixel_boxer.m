function [boundingBox, outBinImg] = shape_pixel_boxer(inpBinImg, adjancency_radius, startX, startY)
%returns a struct that contains the max / min x and y coord for
%a given shape with a pixel at startY, startX

unCheckedPixels = struct('y',{},'x',{});
newPixel = struct('y',startY,'x', startX);
unCheckedPixels = [unCheckedPixels newPixel];
%the y, x format is only to conform with MATLAB, no specific preference or
%need in this algorithm

highX = -999;
highY = -999;
lowX = 99999;
lowY = 99999;

%precalcauting frame x,y adjusts as consts to save time in later loop
frameMin = -floor(adjancency_radius/2);
frameMax = floor(adjancency_radius/2);

while(~isempty(unCheckedPixels))
    %pop next pixel from the unchecked list into current and checked list
    currentPixel = unCheckedPixels(1);
    unCheckedPixels(1) = [];

    %j is y axis, i is x axis
    for j = frameMin:frameMax
        for i = frameMin:frameMax

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

                    %check y,x coord against max/min x and y
                    if(highX < searchX)
                        highX = searchX;
                    end
                    if(lowX > searchX)
                        lowX = searchX;
                    end

                    if(highY < searchY)
                    highY = searchY;
                    end
                    if(lowY > searchY)
                        lowY = searchY;
                    end

                end
            catch except
                continue;
            end

        end
    end
end

boundingBox = struct('x1',lowX,'y1',lowY,'x2',highX,'y2',highY);
outBinImg = inpBinImg;

end