function filteredImg = sobel_filter(inputImg)

s_filter_ver = { {1,2,1} , {0,0,0} ,  {-1,-2,-1} };
s_filter_hor = { {-1,0,1} , {-2,0,2} ,  {-1,0,1} };

inputImg = rgb2gray(inputImg);
filteredImg = inputImg;

end