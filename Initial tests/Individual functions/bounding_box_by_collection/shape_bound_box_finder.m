function boxCoords = shape_bound_box_finder(inpBinImg, adjancency_radius)

adjancency_radius = adjancency_radius + 2;

boundingBoxes = struct('x1',{},'y1',{},'x2',{},'y2',{});

%j is y axis, i is x axis
for j = 1:size(inpBinImg, 1)
    for i = 1:size(inpBinImg, 2)
        if(inpBinImg(j,i))

            %control for using old pixel list or in place comparison
            %function
            funcTypeToUse = 0;
            if(funcTypeToUse == 1)
                pixelSet = shape_pixel_finder(inpBinImg, adjancency_radius, i, j);

                %find highest/lowest x and y
                highX = -999;
                highY = -999;
                lowX = 99999;
                lowY = 99999;
                for idx = 1:numel(pixelSet)

                    if(highX < pixelSet(idx).x)
                        highX = pixelSet(idx).x;
                    end
                    if(lowX > pixelSet(idx).x)
                        lowX = pixelSet(idx).x;
                    end

                    if(highY < pixelSet(idx).y)
                        highY = pixelSet(idx).y;
                    end
                    if(lowY > pixelSet(idx).y)
                        lowY = pixelSet(idx).y;
                    end
                
                    %set all found pixels black
                    inpBinImg(pixelSet(idx).y, pixelSet(idx).x) = false;
                end

                newBox = struct('x1',lowX,'y1',lowY,'x2',highX,'y2',highY);
                boundingBoxes = [boundingBoxes newBox];
            else
                [newBox, inpBinImg] = shape_pixel_boxer(inpBinImg, adjancency_radius, i,j); 
                boundingBoxes = [boundingBoxes newBox];
            end
        end
    end
end

boxCoords = boundingBoxes;

end