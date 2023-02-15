clear;
delete('out/frame/*');
delete('out/base/*');
delete('out/mask/*');

vid = VideoReader('walkingPastCam720p.mp4');
ptr = read(vid);
frameCount = vid.NumFrames;

kernelSize = 19;
backGrndFrameNo = 1;

motionThreshold = 50;

backGrndFrame = sobel_filter(gauss_blur(im2gray(ptr(:, :, :, backGrndFrameNo)), kernelSize));
imwrite(backGrndFrame, 'out/frame/base.png');

for frame = 1:5:frameCount
    if( frame == backGrndFrameNo )
        continue;
    end

    fileNm = strcat(num2str(frame), '.png');
    frameB = sobel_filter(gauss_blur(im2gray(ptr(:, :, :, frame)), kernelSize));
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
    %fprintf('Bounding box: (%i,%i) to (%i,%i)\n', lowestX, lowestY, highestX, highestY);

    %if highest x/y remain out of bounds, then no pixels above the
    %threshold exist, no motion detected, can ignore image

    %mask image outside of motion range (mask whole image for no motion)
    %j = y axis, i = x axis
    if( (lowestX > size(frameB, 2)) || (highestX - lowestX < 100) || (highestY - lowestY < 100) )
        %if no motion detected or motion area is too small (assume noise if this is the case)
        imwrite(ones(size(frameB)), sprintf('out/mask/%s', fileNm));
        fprintf(sprintf('frame %i: none or insignificant motion detected.\n', frame));
    else
        for i = 1:size(frameB, 2)
            for j = 1:size(frameB, 1)
                if( ~((j > lowestY && j < highestY) && (i > lowestX && i < highestX)) )
                    %fprintf('drawing at: (%i,%i)\n', i, j);
                    frameB(j,i) = 0;
                end
            end
        end

        imwrite(frameB, sprintf('out/mask/%s', fileNm));
        fprintf(sprintf('frame %i: motion detected.\n', frame));
    end

    fprintf('%i out of %i completed\n', frame, frameCount);
end


