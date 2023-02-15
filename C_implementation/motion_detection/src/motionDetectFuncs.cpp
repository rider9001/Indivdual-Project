#include "motionDetectFuncs.hpp"

ImgMtx * imageSubtract(ImgMtx * img1, ImgMtx * img2)
{
    //performs image subtraction on two images
    //img1 - img2 = imgOut, image 2 is assumed to be the background or baseline


    if( img1->getHeight() != img2->getHeight() || img1->getWidth() != img2->getWidth() )
    {
        throw std::invalid_argument("Images of different size cannot be subtracted");
    }

    uint8_t ** newMtx = new uint8_t * [ img1->getHeight() ];
	for(int i = 0; i < img1->getHeight(); i++)
	{
		newMtx[i] = new uint8_t[ img1->getWidth() ];
	}

    int outPix;
    for(int y = 0; y < img1->getHeight(); y++)
    {
        for(int x = 0; x < img1->getWidth(); x++)
        {
            outPix = img1->s_getPixel(x,y) - img2->s_getPixel(x,y);
            if(outPix < 0)
            {
                newMtx[y][x] = 0;
            } else {
                newMtx[y][x] = (uint8_t) outPix;
            }
        }
    }

    return new ImgMtx(newMtx, img1->getWidth(), img1->getHeight());
}

boundingBox detectMotion(ImgMtx * motionImg)
{
    //detects motion by tracking an image subtracted image for pixels above a threshold
    //bounds the smallest box containing all pixels above threshold
    //if no pixels above threshold are found, x1/y1 will be at set to uint16 max
    const int threshold = 60;

    boundingBox outBox;
    outBox.x1 = UINT16_MAX;
    outBox.y1 = UINT16_MAX;
    outBox.x2 = 0;
    outBox.y2 = 0;

    //extract the coords for the furthest top-left and low-right pixel
    for(int y = 0; y < motionImg->getHeight(); y++)
    {
        for(int x = 0; x < motionImg->getWidth(); x++)
        {
            if(motionImg->s_getPixel(x,y) > threshold)
            {
                if(x < outBox.x1)
                {outBox.x1 = x;}

                if(y < outBox.y1)
                {outBox.y1 = y;}

                if(x > outBox.x2)
                {outBox.x2 = x;}

                if(y > outBox.y2)
                {outBox.y2 = y;}
            }
        }
    }

    return outBox;
}