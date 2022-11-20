clear;
clf;

inpImgArr = {'plateDay0.5m.jpg', 'plateDay1m.jpg', 'plateDay1.5m.jpg', 'plateDay2m.jpg', 'plateDay2.5m.jpg', 'plateDay3m.jpg'};

gauss_kernel_len = 9;
scalingFactor = 4;
threshold = 110;
adj_strongs_req = 3;
linking_adjacency = 1;
bounding_adjacency = 3;

imgAreas = [];
imgWidths = [];
imgLengths = [];

for imgNo = 1:size(inpImgArr, 2)

curImg = imread(inpImgArr{imgNo});

tic;
[bounding_boxes, binImg] = apply_filter_to_derive_boxes(curImg, gauss_kernel_len, scalingFactor, threshold, adj_strongs_req, linking_adjacency, bounding_adjacency);
toc;
fprintf('---------------------\n');

imshow(binImg);

passList = [];

ratio = 1.58; % 79mm / 50mm
tolerance = 0.15; %percentage value 0-1

for i = 1:numel(bounding_boxes)
    box = bounding_boxes(i);

    boxRatio = (box.y2 - box.y1) / (box.x2 - box.x1);
    %find box ratio and compare to ratio for standard licence plate letter
    if (boxRatio < (ratio + ratio * tolerance)) && (boxRatio > (ratio - ratio * tolerance))
        passList = [passList, bounding_boxes(i)];
    end
end

%red areas pass the ratio filter
for i = 1:numel(passList)
        box = passList(i);
       rectangle('Position', [box.x1 box.y1 (box.x2-box.x1) (box.y2-box.y1)], ...
        'LineWidth', 1, 'EdgeColor', 'r', 'LineStyle', '--');
end

saveas(gcf, sprintf('out/ratioPass%i.png', imgNo));

bounding_boxes = passList;
passList = [];

%get image width
imWidth = size(binImg, 2);

%bounding box size limits percent of image width
lowBoundLim = imWidth * 0.01;
highBoundLim = imWidth * 0.1;

%crude width filtering
for i = 1:numel(bounding_boxes)
    box = bounding_boxes(i);

    if ((box.x2 - box.x1) > lowBoundLim) && ((box.x2 - box.x1) < highBoundLim)
        passList = [passList, bounding_boxes(i)];
    end
end

%yellow areas pass the size filter
for i = 1:numel(passList)
        box = passList(i);
       rectangle('Position', [box.x1 box.y1 (box.x2-box.x1) (box.y2-box.y1)], ...
        'LineWidth', 1, 'EdgeColor', 'y', 'LineStyle', '--');
end

saveas(gcf, sprintf('out/widthPass%i.png', imgNo));

bounding_boxes = passList;
passList = [];

yCoordList = [];
areaList = [];

for i = 1:numel(bounding_boxes)
    yCoordList = [yCoordList, bounding_boxes(i).y1];
    areaList = [areaList ((bounding_boxes(i).y2 - bounding_boxes(i).y1)*(bounding_boxes(i).x2-bounding_boxes(i).x1)) ];
end

%round to nearest N multiple
unitForRoundingArea = 2500;

for i = 1:numel(bounding_boxes)
    j = 0;
    %increase multiple of N until N and N+1 range contains datapoint
    while( (j+1) * unitForRoundingArea < areaList(i) )
        j = j + 1;
    end

    %find if the upper or lower bound of range is closer to data point, set
    %closest to be new value
    if( ( (j+1)*unitForRoundingArea - areaList(i) ) < ( areaList(i) - j*unitForRoundingArea ) )
        %upper bound is closer
       areaList(i) = (j+1) * unitForRoundingArea;
    else
       areaList(i) = j * unitForRoundingArea;
    end
end

unitForRoundingYcoord = 150;

for i = 1:numel(bounding_boxes)
    j = 0;
    %increase multiple of N until N and N+1 range contains datapoint
    while( (j+1) * unitForRoundingYcoord < yCoordList(i) )
        j = j + 1;
    end

    %find if the upper or lower bound of range is closer to data point, set
    %closest to be new value
    if( ( (j+1)*unitForRoundingYcoord - yCoordList(i) ) < ( yCoordList(i) - j*unitForRoundingYcoord ) )
        %upper bound is closer
       yCoordList(i) = (j+1) * unitForRoundingYcoord;
    else
       yCoordList(i) = j * unitForRoundingYcoord;
    end
end

yCoordMode = mode(yCoordList);
areaMode = mode(areaList);

for i = 1:numel(bounding_boxes)
    box = bounding_boxes(i);
    if (areaList(i) == areaMode && yCoordMode == yCoordList(i))
        passList = [passList, bounding_boxes(i)];
    end
end

x1 = 0;
y1 = 0;
wid = 0;
high = 0;

for i = 1:numel(passList)
    x1 = x1 + passList(i).x1;
    y1 = y1 + passList(i).y1;
    wid = wid + passList(i).x2 - passList(i).x1;
    high = high + passList(i).y2 - passList(i).y1;
end


x1 = x1 / numel(passList);
wid = wid / numel(passList);
y1 = y1 / numel(passList);
high = high / numel(passList);

%green boxes are assumed to be the licence plate letters
for i = 1:numel(passList)
    box = passList(i);
       rectangle('Position', [box.x1 box.y1 (box.x2-box.x1) (box.y2-box.y1)], ...
        'LineWidth', 1, 'EdgeColor', 'g', 'LineStyle', '--');
end

saveas(gcf, sprintf('out/modePass%i.png', imgNo));

%blue is the average of the green boxes
rectangle('Position', [x1, y1, wid, high], ...
'LineWidth', 1, 'EdgeColor', 'b', 'LineStyle', '--');

saveas(gcf, sprintf('out/finalBox%i.png', imgNo));

avgBoxArea = wid * high;

fprintf(sprintf('Average box area: %d pix^2\n', avgBoxArea));
fprintf(sprintf('Average box width: %d pix\n', wid));
fprintf(sprintf('Average box length: %d pix\n', high));
fprintf(sprintf('No of passing bounding boxes: %i\n', numel(passList)));
fprintf('----------END----------\n');

imgAreas = [imgAreas avgBoxArea];
imgLengths = [imgLengths high];
imgWidths = [imgWidths wid];

end


semilogy(1:1:6, imgAreas, 1:1:6, imgWidths, 1:1:6, imgLengths);
grid on;
legend('Area', 'Width', 'Length');
xlabel('Image no');
ylabel('Size in pixels');

saveas(gcf, 'out/sizeComparison.png')