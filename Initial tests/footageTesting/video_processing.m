clear;
delete('out\frames\*');
delete('out\base\*');

vid = VideoReader('sourceVideo\wavingPlate1080p.mp4');
ptr = read(vid);
frameCount = vid.NumFrames;

%control variables
thres = 210;
targetShow = -1;
frameStep = 15;

for i = 1:frameStep:frameCount
    fileNm = strcat(num2str(i), '.png');
    frame = ptr(:, :, :, i);
    imwrite(frame, sprintf('out/base/%s', fileNm));

    %frame = sobel_filter(frame, thres);
    frame = im2gray(frame);
    frame = edge(frame, 'canny');

    if i == targetShow
        imshow(frame);
        %find both black and white regions
        stats = [regionprops(frame); regionprops(not(frame))];
        hold on;
        for j = 1:numel(stats)
            rectangle('Position', stats(j).BoundingBox, ...
            'Linewidth', 1, 'EdgeColor', 'r', 'LineStyle', '--');
        end
    end

    imwrite(frame, sprintf('out/frames/%s', fileNm));
    fprintf('%i out of %i completed\n', i, frameCount);
end

clear;