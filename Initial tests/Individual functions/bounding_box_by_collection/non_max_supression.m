function suppressed_Img = non_max_supression(inpMat)
%takes the output from the sobel filter and uses directionality to supress
%non maximums

suppressed_Img = zeros( size(inpMat,1), size(inpMat, 2) );

supressionKernelLen = 3;

%j is y axis, i is x axis
for j = ceil(supressionKernelLen / 2) : size(inpMat, 2) - ceil(supressionKernelLen / 2)
    for i = ceil(supressionKernelLen / 2) : size(inpMat, 1) - ceil(supressionKernelLen / 2)
        %fprintf(sprintf('Viewing %i,%i\n', j, i));
        switch inpMat(i,j,2)
            case 0
                %up-down direction
                if(inpMat(i,j,1) < inpMat(i,j+1,1) || inpMat(i,j,1) < inpMat(i,j-1,1))
                    suppressed_Img(i,j) = 0;
                else
                    suppressed_Img(i,j) = inpMat(i,j,1);
                end
            case 1
                %direction is left-right
                if(inpMat(i,j,1) < inpMat(i+1,j,1) || inpMat(i,j,1) < inpMat(i-1,j,1))
                    suppressed_Img(i,j) = 0;
                else
                    suppressed_Img(i,j) = inpMat(i,j,1);
                end
            case 2
                %direction is upRight-downLeft
                if(inpMat(i,j,1) < inpMat(i-1,j+1,1) || inpMat(i,j,1) < inpMat(i+1,j-1,1))
                    suppressed_Img(i,j) = 0;
                else
                    suppressed_Img(i,j) = inpMat(i,j,1);
                end
            case 3
                %direction is upLeft-downRight
                if(inpMat(i,j,1) < inpMat(i+1,j-1,1) || inpMat(i,j,1) < inpMat(i-1,j+1,1))
                    suppressed_Img(i,j) = 0;
                else
                    suppressed_Img(i,j) = inpMat(i,j,1);
                end
        end
    end
end

suppressed_Img = uint8(suppressed_Img);

end