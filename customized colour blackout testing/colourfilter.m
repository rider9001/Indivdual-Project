% ----------------------------------------------------------------------- %
%                        C O L O R    F I L T E R                         %
% ----------------------------------------------------------------------- %
% This function modifies a given image in order to keep a specific hue    %
% (given too) and to desaturate the rest of the image. This procedure     %
% originates a image with black and white colormap, excluding the parts   %
% colored with that hue.                                                  %
%                                                                         %
%   Input parameters:                                                     %
%       · image:   input image with double format (0 to 1 values).        %
%       · range:   range of hues vector to maintain with the following    %
%                  format: [range_min, range_max], whose mins and max     %
%                  belongs to [0,360]º.                                   %
%         Note: Red hue appears in both ends of the hue spectrum (called H%
%         in HSV). If red hue is desired, it is necessary to indicate a   %
%         higher value in 'range_min' than 'range_max'. This way assumes  %
%         that the next value of 360º is 0º, and so on.                   %
%           Example: range = [350, 50];                                   %
%                                                                         %
%   Output variables:                                                     %
%       · I:       modified image.                                        %
% ----------------------------------------------------------------------- %
%   Author:  Víctor Martínez Cagigal                                      %
%   Date:    23/02/2015                                                   %
%   E-mail:  victorlawliet (dot) gmail (dot) com                          %
% ----------------------------------------------------------------------- %
function I = colourfilter(image, range)
    % RGB to HSV conversion
    I = rgb2hsv(image);         
    
    % Normalization range between 0 and 1
    range = range./360;
    
    % Mask creation
    if(size(range,1) > 1), error('Error. Range matriz has too many rows.'); end
    if(size(range,2) > 2), error('Error. Range matriz has too many columns.'); end
    if(range(1) > range(2))
        % Red hue case
        mask = (I(:,:,1)>range(1) & (I(:,:,1)<=1)) + (I(:,:,1)<range(2) & (I(:,:,1)>=0));
    else
        % Regular case
        mask = (I(:,:,1)>range(1)) & (I(:,:,1)<range(2));
    end
    
    % Saturation is modified according to the mask
    %I(:,:,2) = mask .* I(:,:,2);
    
    % HSV to RGB conversion
    I = im2double(hsv2rgb(I));

    %filter all non matching pixels to white
    %I(:,:,1) = mask .* I(:,:,1);
    %I(:,:,2) = mask .* I(:,:,2);
    %I(:,:,3) = mask .* I(:,:,3);

    maskPosRepl = 1;
    maskNegRepl = 0;

    for i = 1:size(I,1)
        for j = 1:size(I,2)
            if mask(i,j) == 1
                I(i,j,1) = maskPosRepl;
                I(i,j,2) = maskPosRepl;
                I(i,j,3) = maskPosRepl;
            else
                I(i,j,1) = maskNegRepl;
                I(i,j,2) = maskNegRepl;
                I(i,j,3) = maskNegRepl;
            end
        end
    end
    
end