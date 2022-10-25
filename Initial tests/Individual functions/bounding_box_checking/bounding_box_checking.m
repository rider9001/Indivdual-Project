clear;
inpImg = double(im2gray(imread('img2.jpg')));

kernelSize = 19;
I_out_blur = gauss_blur(inpImg, kernelSize);

thresh = 150;
%I_out_edge = sobel_filter(I_out_blur, thresh);
I_out_edge = edge(I_out_blur, 'sobel');

imshow(I_out_edge);
%find both black and white regions
boundedAreas = [regionprops(I_out_edge); regionprops(not(I_out_edge))];

%help = stats(j).BoundingBox;
%bounding box index: [origin x, origin y, width, height]

passList = [];

ratio = 1.58; % 79mm / 50mm
tolerance = 0.2; %percentage value 0-1

%bounding box size limits percent of image width
lowBoundLim = 0.01;
highBoundLim = 0.04;

for i = 1:numel(boundedAreas)
    box = boundedAreas(i).BoundingBox;

    boxRatio = box(4) / box(3);
    %find box ratio and compare to ratio for standard licence plate letter
    if (boxRatio < (ratio + ratio * tolerance)) && (boxRatio > (ratio - ratio * tolerance))
        passList = [passList, boundedAreas(i)];
    end
end

%red areas pass the ratio filter
for i = 1:numel(passList)
       rectangle('Position', passList(i).BoundingBox, ...
        'LineWidth', 1, 'EdgeColor', 'r', 'LineStyle', '--');
end

boundedAreas = passList;
passList = [];

imWidth = size(I_out_edge, 2);

%crude width filtering
for i = 1:numel(boundedAreas)
    box = boundedAreas(i).BoundingBox;

    if (box(3) > imWidth * lowBoundLim) && (box(3) < imWidth * highBoundLim)
        passList = [passList, boundedAreas(i)];
    end
end

%yellow areas pass the size filter
for i = 1:numel(passList)
       rectangle('Position', passList(i).BoundingBox, ...
        'LineWidth', 1, 'EdgeColor', 'y', 'LineStyle', '--');
end

boundedAreas = passList;
passList = [];

%round to nearest 100 and take mode intersection of x and y
xCoordList = [];
yCoordList = [];

for i = 1:numel(boundedAreas)
    xCoordList = [xCoordList, boundedAreas(i).BoundingBox(1)];
    yCoordList = [yCoordList, boundedAreas(i).BoundingBox(2)];
end

%round each list to nearest 100 for y, 1000 for x
xCoordList = round(xCoordList, -3);
yCoordList = round(yCoordList, -3);
xCoordMode = mode(xCoordList);
yCoordMode = mode(yCoordList);

for i = 1:numel(boundedAreas)
    box = boundedAreas(i).BoundingBox;
    %if bounding box has x,y origin of the mode of all bounding boxes
    %rounded to 100
    if (round(box(1), -3) == xCoordMode) && (round(box(2), -3) == yCoordMode)
        passList = [passList, boundedAreas(i)];
    end
end

x1 = 0;
y1 = 0;
wid = 0;
high = 0;

for i = 1:numel(passList)
    x1 = x1 + passList(i).BoundingBox(1);
    y1 = y1 + passList(i).BoundingBox(2);
    wid = wid + passList(i).BoundingBox(3);
    high = high + passList(i).BoundingBox(4);
end


x1 = x1 / numel(passList);
wid = wid / numel(passList);
y1 = y1 / numel(passList);
high = high / numel(passList);

%green pixels are assumed to be the licence plate characters
for i = 1:numel(passList)
       rectangle('Position', passList(i).BoundingBox, ...
        'LineWidth', 1, 'EdgeColor', 'g', 'LineStyle', '--');
end

%blue is the average of the green boxes
rectangle('Position', [x1, y1, wid, high], ...
'LineWidth', 1, 'EdgeColor', 'b', 'LineStyle', '--');

