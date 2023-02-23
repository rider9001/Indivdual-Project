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
    const int threshold = 50;

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

boundingBox HFTstDetectMotion(ImgMtx * motionImg)
{
    /*
        Issues with high frequency noise causing image detects, in other words, slight motion
        want to enforce a higher motion requirement on bounding box masking
        plan is to divide image into square sectors and map motion onto these

        take a square and detect for motion, if whatever % of pixels are above threshold
        then mask is set to inculde this sector
        final box gets lowest/highest x/y of all passing sectors
    */

    const int sectorLen = 50;
    //sector length in pixels, must be a factor of both image width and height

    boundingBox outBox;
    outBox.x1 = UINT16_MAX;
    outBox.y1 = UINT16_MAX;
    outBox.x2 = 0;
    outBox.y2 = 0;

    unsigned int sectorPassCount = 0;

    if( (motionImg->getHeight() % sectorLen != 0) or (motionImg->getWidth() % sectorLen != 0) )
    {
        throw std::invalid_argument("sector length is invalid");
    }

    for(int yOrigin = 0; yOrigin < motionImg->getHeight(); yOrigin += sectorLen)
    {
        for(int xOrigin = 0; xOrigin < motionImg->getWidth(); xOrigin += sectorLen)
        {
            if( sectorPass(motionImg, xOrigin, yOrigin, sectorLen) )
            {
                sectorPassCount++;

                //note that new high x/y values are offset to the BR edge of the sector
                if(xOrigin < outBox.x1)
                {outBox.x1 = xOrigin;}

                if(yOrigin < outBox.y1)
                {outBox.y1 = yOrigin;}

                if( (xOrigin + sectorLen) > outBox.x2)
                {outBox.x2 = xOrigin + sectorLen;}

                if( (yOrigin + sectorLen) > outBox.y2)
                {outBox.y2 = yOrigin + sectorLen;}
            }
        }
    }

    if(sectorPassCount < 5)
    {
        //reject image for motion if too few sectors are detected
        outBox.x1 = UINT16_MAX;
        outBox.y1 = UINT16_MAX;
        outBox.x2 = 0;
        outBox.y2 = 0;
    }

    return outBox;
}

bool sectorPass(ImgMtx * motionImg, int xOrigin, int yOrigin, const int sectorLen)
{
    const float passFactor = 0.125f; 
    //percentage of pixels passing threshold to consider a sector motion postive
    const unsigned int threshold = 40;
    //threshold value for motion detection (0-255)
    const unsigned int passPixCount = sectorLen*sectorLen * passFactor;
    //total pixel count needed for a pass

    unsigned int passCount = 0;

    for(int y = yOrigin; y < yOrigin + sectorLen; y++)
    {
        for(int x = xOrigin; x < xOrigin + sectorLen; x++)
        {
            if(motionImg->s_getPixel(x,y) > threshold)
            {passCount++;}
        }
    }

    cout << "Pixels pass/needed: " << passCount << '/' << passPixCount << endl;

    return passCount >= passPixCount;
}

ImgMtx * MaskImg(ImgMtx * img, boundingBox mask)
{
    //function modifies an image to mask (0 out all pixels) not included
    //within the box defined by the mask

    //check mask has coords inside of source image
    if( (mask.x1 < 0) or (mask.y1 < 0) or (mask.x2 > img->getWidth()) or (mask.y2 > img->getHeight()) )
    {
        throw std::invalid_argument("Mask has invalid coords");
    }

    //create new matrix for pixels
    uint8_t ** newMtx = new uint8_t * [ img->getHeight() ];
	for(int i = 0; i < img->getHeight(); i++)
	{
		newMtx[i] = new uint8_t[ img->getWidth() ];
	}

    //for all coords (0,0) refers to top left pixel
    for(int y = 0; y < img->getHeight(); y++)
    {
        for(int x = 0; x < img->getWidth(); x++)
        {
            //check if x/y coord is within the mask
            if( (y > mask.y1) and (y < mask.y2) and (x > mask.x1) and (x < mask.x2) )
            {
                newMtx[y][x] = img->s_getPixel(x,y);
            } else {
                newMtx[y][x] = 0;
            }
        }
    }

    return new ImgMtx(newMtx, img->getWidth(), img->getHeight());
}