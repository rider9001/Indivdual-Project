clear;
delete('out/frame/*');
delete('out/base/*');
delete('out/mask/*');

vid = VideoReader('plateHeadOnMotion1080p.mp4');
ptr = read(vid);
frameCount = vid.NumFrames;

kernelSize = 19;
backGrndFrameNo = 1;

motionThreshold = 50;

backGrndFrame = gauss_blur(im2gray(ptr(:, :, :, backGrndFrameNo)), kernelSize);
imwrite(backGrndFrame, 'out/frame/base.png');

for frame = 1:10:frameCount
    if( frame == backGrndFrameNo )
        continue;
    end

    fileNm = strcat(num2str(frame), '.png');
    frameB = gauss_blur(im2gray(ptr(:, :, :, frame)), kernelSize);
    imwrite(frameB, sprintf('out/base/%s', fileNm));

    frameMot = imsubtract(frameB, backGrndFrame);
    imwrite(frameMot, sprintf('out/frame/%s', fileNm));

    lowestX = 9999999;
    lowestY = 9999999;
    highestX = -1;
    highestY = -1;

    %j = y axis, i = x axis
    for i = 1:size(frameB, 2)
        for j = 1:size(frameB, 1)
            if (frameMot(j,i) > motionThreshold)
                %check for x coord
                if(i < lowestX)
                    lowestX = i; 
                end
                if(i > highestX)
                    highestX = i;
                end

                %check for y coord
                if(j < lowestY)
                    lowestY = j; 
                end
                if(j > highestY)
                    highestY = j;
                end
            end
        end
    end
    fprintf('Bounding box: (%i,%i) to (%i,%i)\n', lowestX, lowestY, highestX, highestY);

    %if highest x/y remain out of bounds, then no pixels above the
    %threshold exist, no motion detected, can toss image

    %mask image outside of motion range (mask whole image for no motion)
    %j = y axis, i = x axis
    if(lowestX > size(frameB, 2))
        out = zeros(size(frameB));
        imwrite(out, sprintf('out/mask/%s', fileNm));
    else
        for i = 1:size(frameB, 2)
            for j = 1:size(frameB, 1)
                if( (j > highestY || j > lowestY) || (i > highestX || i < lowestX) )
                    frameB(j,i) = 0;
                end
            end
        end

        imwrite(frameB, sprintf('out/mask/%s', fileNm));
    end

    fprintf('%i out of %i completed\n', frame, frameCount);
end


