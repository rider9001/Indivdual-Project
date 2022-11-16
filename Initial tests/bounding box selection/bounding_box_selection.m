clear;

inpImg = imread('plateDay2.5m.jpg');

gauss_kernel_len = 9;
scalingFactor = 4;
threshold = 110;
adj_strongs_req = 3;
linking_adjacency = 1;
bounding_adjacency = 3;

tic;
[bounding_boxes, binImg] = apply_filter_to_derive_boxes(inpImg, gauss_kernel_len, scalingFactor, threshold, adj_strongs_req, linking_adjacency, bounding_adjacency);
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

saveas(gcf, 'out/ratioPass.png');

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

saveas(gcf, 'out/widthPass.png');

bounding_boxes = passList;
passList = [];

%round to nearest 100 and take mode intersection of x and y
xCoordList = [];
yCoordList = [];
areaList = [];

for i = 1:numel(bounding_boxes)
    xCoordList = [xCoordList, bounding_boxes(i).x1];
    yCoordList = [yCoordList, bounding_boxes(i).y1];
    areaList = [areaList ((bounding_boxes(i).y2 - bounding_boxes(i).y1)*(bounding_boxes(i).x2-bounding_boxes(i).x1)) ];
end

%round to nearest N multiple
unitForRoundingArea = 2000;

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

unitForRoundingYcoord = 300;

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

% xCoordList = round(xCoordList, roundingX);
% yCoordList = round(yCoordList, roundingY);

xCoordMode = mode(xCoordList);
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

%green pixels are assumed to be the licence plate characters
for i = 1:numel(passList)
    box = passList(i);
       rectangle('Position', [box.x1 box.y1 (box.x2-box.x1) (box.y2-box.y1)], ...
        'LineWidth', 1, 'EdgeColor', 'g', 'LineStyle', '--');
end

saveas(gcf, 'out/modePass.png');

%blue is the average of the green boxes
rectangle('Position', [x1, y1, wid, high], ...
'LineWidth', 1, 'EdgeColor', 'b', 'LineStyle', '--');

saveas(gcf, 'out/finalBox.png');