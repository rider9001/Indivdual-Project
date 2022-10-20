clear;
inpImg = double(im2gray(imread('img2.jpg')));

kernel = 19;
I_out_blur = gauss_blur(inpImg, kernel);

thresh = 150;
%I_out_edge = sobel_filter(I_out_blur, thresh);
I_out_edge = edge(I_out_blur, 'sobel');

imshow(I_out_edge);
%find both black and white regions
stats = [regionprops(I_out_edge); regionprops(not(I_out_edge))];

%help = stats(j).BoundingBox;
%bounding box index: [origin x, origin y, width, height]

ratio = 1.58; % 79mm / 50mm
tolerance = 0.2; %percentage value 0-1

validBoxes = [];

hold on;
for j = 1:numel(stats)

    box = stats(j).BoundingBox;
    boxRatio = box(4) / box(3);
    if (boxRatio < (ratio + ratio * tolerance)) && (boxRatio > (ratio - ratio * tolerance))
       validBoxes = [validBoxes stats(j)];
       rectangle('Position', stats(j).BoundingBox, ...
       'LineWidth', 1, 'EdgeColor', 'g', 'LineStyle', '--');
    end
end

x1 = 0;
y1 = 0;
wid = 0;
high = 0;

for i = 1:numel(validBoxes)
    x1 = x1 + validBoxes(i).BoundingBox(1);
    y1 = y1 + validBoxes(i).BoundingBox(2);
    wid = wid + validBoxes(i).BoundingBox(3);
    high = high + validBoxes(i).BoundingBox(4);
end


x1 = x1 / numel(validBoxes);
wid = wid / numel(validBoxes);
y1 = y1 / numel(validBoxes);
high = high / numel(validBoxes);

rectangle('Position', [x1, y1, wid, high], ...
'LineWidth', 1, 'EdgeColor', 'b', 'LineStyle', '--');

