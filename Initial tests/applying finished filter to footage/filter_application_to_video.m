clear;
delete('out/baseFrames/*');
delete('out/outFrames/*');

vid = VideoReader('plateMovingBacktoFront720p.mp4');
ptr = read(vid);
frameCount = vid.NumFrames;

gauss_kernel_len = 9;
scalingFactor = 4;
threshold = 110;
adj_strongs_req = 3;
linking_adjacency = 1;
bounding_adjacency = 3;

frameStep = 5;

for i = 1:frameStep:frameCount
    clf;
    partfileNm = strcat(num2str(i), '.png');
    frame = ptr(:, :, :, i);
    imwrite(frame, sprintf('out/baseFrames/base%s', partfileNm));

    
    tic;
    [boxCoords, binImg] = apply_filter_to_derive_boxes(frame, gauss_kernel_len, scalingFactor, threshold, adj_strongs_req, linking_adjacency, bounding_adjacency);
    fprintf(sprintf('Frame %i filtered in:\n', i));
    toc;
    imshow(binImg);

    for idx = 1:numel(boxCoords)
        currBox = [boxCoords(idx).x1, boxCoords(idx).y1, (boxCoords(idx).x2 - boxCoords(idx).x1), (boxCoords(idx).y2 - boxCoords(idx).y1)];
        rectangle('Position', currBox, ...
         'LineWidth', 1, 'EdgeColor', 'r', 'LineStyle', '--');
    end

    saveas(gcf, sprintf('out/outFrames/bin%s', partfileNm));
    fprintf(sprintf('frame %i / %i completed\n', i, frameCount));
    fprintf('----------------------\n');
end