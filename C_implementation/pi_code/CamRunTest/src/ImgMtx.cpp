/*
	Class intended to store all functions and information relating to image transforms and
	image reading/writing

	Staging the various filters is done by loading an image into the object then calling
	the relevant routines to perform the filtering on the data in place inside the object

	Calling writeImg will output the current state of the pixMtx as a jpg file with filename given
*/

#include "ImgMtx.h"

ImgMtx::~ImgMtx()
{
    //cleans all non constant data pointer locations, if no image loaded then
    //will skip this step to prevent undefined behaviour trying to delete a
    //non-existent memory block

    if(pixMtx != NULL)
    {
        for(int i = 0; i < height; i++)
        {
            delete[] pixMtx[i];
        }
        delete[] pixMtx;
    }

    if(dirMtx != NULL)
    {
        for(int i = 0; i < height; i++)
        {
            delete[] dirMtx[i];
        }
        delete[] dirMtx;
    }
}

ImgMtx::ImgMtx()
{
    pixMtx = NULL;
    dirMtx = NULL;
    width = -1;
    height = -1;
    originFilename = "NO FILE LOADED";
}

ImgMtx::ImgMtx(uint8_t ** mtxPtr, int widthIn, int heightIn)
{
    originFilename = "Name not given";
    pixMtx = mtxPtr;
    dirMtx = NULL;
    width = widthIn;
    height = heightIn;
}

ImgMtx::ImgMtx(uint8_t ** mtxPtr, int widthIn, int heightIn, const char * fileNmIn)
{
    originFilename = fileNmIn;
    pixMtx = mtxPtr;
    dirMtx = NULL;
    width = widthIn;
    height = heightIn;
}

ImgMtx::ImgMtx(const char * filename)
{
    //set extra arrays to empty until created
    dirMtx = NULL;

	//read file and get meta-data
	int x,y,n;
	uint8_t *data = stbi_load(filename, &x, &y, &n, 0);

	if(data == NULL)
	{
		//ptr returns as null if img read fails
		throw std::invalid_argument("ERROR: failed to read file '" + (string)filename + "', file may be missing.");
	}

	if(n != 3)
	{
		//image data not valid to become grayscale if not containing RGB pixel data
		throw std::invalid_argument("ERROR: file '" + (string)filename + "not in RGB 3 byte per pix format.");
	}

	//if passed basic checks, begin reading into matrix
	width = x;
	height = y;
	originFilename = filename;

	pixMtx = new uint8_t*[height];
	for(int i = 0; i < height; i++)
	{
		pixMtx[i] = new uint8_t[width];
	}
	//img coords are (row, col)
	//cout << originFilename << " is " << pixMtx[0].size() << " by " << pixMtx.size() << " pixels." << endl;

	unsigned long pixIdx;
	for(int j = 0; j < height; j++)
	{
		pixIdx = j * width * 3;
		for(int i = 0; i < width; i++)
		{
			pixMtx[j][i] = grayscalePixel(data[pixIdx], data[pixIdx+1], data[pixIdx+2]);
			//cout << (int)pixMtx.at(j).at(i) << " from index: " << pixIdx << endl;
			pixIdx += 3;
		}
	}

	//delete stb image data
	stbi_image_free(data);
}

void ImgMtx::overWrtPixMtx(uint8_t ** sourceMtx)
{
    //deletes old matrix pointers and replaces main pointer with new matrix pointer
    //!!!assumes that matrices are all the size of the image!!!

    //check if data is loaded into default pointers
    if(pixMtx != NULL)
    {
        for(int i = 0; i < height; i++)
        {
            delete[] pixMtx[i];
        }
        delete pixMtx;
    }

    pixMtx = sourceMtx;
}

void ImgMtx::overWrtDirMtx(uint8_t ** sourceMtx)
{
    //deletes old matrix pointers and replaces main pointer with new matrix pointer
    //!!!assumes that matrices are all the size of the image!!!

    //check if data is loaded into default pointers
    if(dirMtx != NULL)
    {
        cout << "dirMtx wiping" << endl;
        for(int i = 0; i < height; i++)
        {
            delete[] dirMtx[i];
        }
        delete dirMtx;
    }

    dirMtx = sourceMtx;
}

uint8_t ImgMtx::s_getPixel(int x, int y)
{
    if( x < 0 || y < 0 || x >= width || y >= height )
    {
        throw std::invalid_argument("ERROR: index: (" + to_string(x) + ", " + to_string(y) + ") is outside valid range of image size: " + to_string(width) + " by " + to_string(height));
    }
    //public facing version of pixel get, allowed to throw exception if out of range
    return pixMtx[y][x];
}

uint8_t ImgMtx::getPixel(int x, int y)
{
    //unsafe internal version of pixel get, returns 0 on out of bounds coordiante
    if( x < 0 || y < 0 || x >= width || y >= height )
    {return 0;}

    return pixMtx[y][x];
}

bool ImgMtx::getImLoaded()
{
    return pixMtx != NULL;
}

int ImgMtx::getWidth()
{
    return width;
}

int ImgMtx::getHeight()
{
    return height;
}

filterStage ImgMtx::getStage()
{
    return stage;
}

uint8_t ImgMtx::grayscalePixel(uint8_t R, uint8_t G, uint8_t B)
{
	R = ( (R<<1) + R ) / 10; // 0.3 * R
	G = ( (G<<2) + (G<<1) ) / 10; // 0.6 * G
	B = B / 10; // 0.1 * B

	return R + G + B;
}

void ImgMtx::gaussBlur()
{
    //returns a new ImgMtx obj that contains the gaussian blur of the current objects image
    if(pixMtx == NULL)
    {
        throw std::invalid_argument("ERROR: no image data loaded.");
    }

    if(stage != Grayscale)
    {
        cout << "WARNING: Filter stage out of order, performing gaussian filter on a stage other than grayscale." << endl;
    }

    //define coefficient list for kernel, will probably make this dynamic in future
    //note that the kernel (and thus the coeff list can only be even)
    const int coeffListLen = 9;
    const uint8_t coeffList[coeffListLen] = {1,8,28,56,70,56,28,8,1};
    
    const int divideShift = 17; 
    //divide by 2^18, sum of all coefficients squared (N>>divideShift)
    //divide shift may be lower than above to brighten image after guass filter

    //create output matrix
    uint8_t ** gaussMtx = new uint8_t*[height];
    for(int i = 0; i < height; i++)
    {
        gaussMtx[i] = new uint8_t[width];
    }

    //create upper and lower offsets to apply convolution filter to matrix
    int lowerKerCorner = -floor(coeffListLen / 2);
    int upperKerCorner = floor(coeffListLen / 2);

    for(int y = 0; y < height; y++)
    {
        for(int x = 0; x < width; x++)
        {
            uint32_t pixTotal = 0; //32 bits is more than enough to store largest possible value (2^10)^2 * 255
            for(int j = lowerKerCorner; j <= upperKerCorner; j++)
            {
                for(int i = lowerKerCorner; i <= upperKerCorner; i++)
                {
                    //get pixel value at the offset location
                    uint8_t pixVal = getPixel( i+x , j+y );
                    //skip calculation if pixVal is zero
                    if(pixVal != 0)
                    {
                        //calculate blur value, array offset is used to return j/i to 0:coeffListLen range from lowerKerCorner:upperKerCorner
                        pixTotal += pixVal * coeffList[j+upperKerCorner] * coeffList[i+upperKerCorner];
                    }
                }
            }
            //shift result and take LS 8 bits
            gaussMtx[y][x] = (pixTotal >> divideShift) & 0xFF;
        }
    }

    //delete old image matrix and set core pointer to new matrix
    overWrtPixMtx(gaussMtx);

    //set filter stage to next point
    stage = GaussFiltered;
}

void ImgMtx::SobelFil()
{
    //sobel filter the image matrix and populate the angle matrix as well

    if(stage != GaussFiltered)
    {
        cout << "WARNING: Sobel filter is being performed out of order, image likely to be corrupted." << endl;
    }

    //define sobel convolution filters
    const int fil_ver[9] = {-1,0,1,-2,0,2,-1,0,1};
    const int fil_hor[9] = {-1,-2,-1,0,0,0,1,2,1};
    const int SobelFilLen = 3; //side length of kernel, in every 3 entires on the above
    //list is a new line in the filter matrix, hence a 3x3 kernel

    //create new image matrix
    uint8_t ** sobelMtx = new uint8_t*[height];
    for(int i = 0; i < height; i++)
    {
        sobelMtx[i] = new uint8_t[width];
    }

    //create direction code matrix
    uint8_t ** angCalcMtx = new uint8_t*[height];
    for(int i = 0; i < height; i++)
    {
        angCalcMtx[i] = new uint8_t[width];
    }

    int lowerKerCorner = -floor(SobelFilLen / 2);
    int upperKerCorner = floor(SobelFilLen / 2);

    for(int y = 0; y < height; y++)
    {
        for(int x = 0; x < width; x++)
        {
            //reset per pixel ver/hor weight values
            int16_t VerW, HorW;
            VerW = 0;
            HorW = 0;

            for(int j = lowerKerCorner; j <= upperKerCorner; j++)
            {
                for(int i = lowerKerCorner; i <= upperKerCorner; i++)
                {
                    uint8_t pixVal = getPixel( x+i , y+j );
                    //j shifts the row of the kernel, i indexes the 3 long row
                    //const addition is to shift range to 0:2
                    uint8_t filIdx = ( (j+upperKerCorner) * 3) + (i+upperKerCorner);

                    //calculate ver weight for pixel
                    //skip calculation if any used value is 0
                    if(pixVal != 0 && fil_ver[filIdx] != 0)
                    {
                        VerW += pixVal * fil_ver[filIdx];
                    }

                    //calculate hor weight for pixel
                    if(pixVal != 0 && fil_hor[filIdx] != 0)
                    {
                        HorW += pixVal * fil_hor[filIdx];
                    }
                }
            }
            //find magnitude and direction of combined ver/hor weights, limit to 255 max
            uint8_t outPix;
            if( (abs(VerW) + abs(HorW) ) > 255)
            {outPix = 255;}
            else
            {outPix = abs(VerW) + abs(HorW);}

            sobelMtx[y][x] = outPix;
            angCalcMtx[y][x] = aproxDir(VerW, HorW);
        }
    }

    //overwrite pixel and dir matrix
    overWrtPixMtx(sobelMtx);
    overWrtDirMtx(angCalcMtx);

    //advance stage flag of filter
    stage = SobelFiltered;
}

uint8_t ImgMtx::aproxDir(int16_t Ver, int16_t Hor)
{
    //approximate arctan(Hor/Ver) to 4 possible values
    if(Hor == 0)
    {
        //direction is up-down
        return 0;
    }
    if(Ver == 0)
    {
        //direction is left-right
        return 1;
    }
    if( ((Ver < 0) & (Hor < 0)) || ((Ver > 0) & (Hor > 0)) )
    {
        //same sign check, direction is upRight-downLeft
        return 2;
    }

    //if none pass, direction is upLeft-downRight
    return 3;
}

void ImgMtx::nonMaxSupress()
{
    //performs non-max suppression on the pixMtx based on the dirMtx directions
    //calcualted in the sobel filter stage

    if(dirMtx == NULL)
    {
        throw std::invalid_argument("ERROR: no direction data calculated, perform a sobel filter first.");
    }

    if(stage != SobelFiltered)
    {
        cout << "WARNING: Non-max suppression is being performed out of order, image likely to be corrupted." << endl;
    }

    //create output image matrix
    uint8_t ** supresMtx = new uint8_t*[height];
    for(int i = 0; i < height; i++)
    {
        supresMtx[i] = new uint8_t[width];
    }

    for(int y = 0; y < height; y++)
    {
        for(int x = 0; x < width; x++)
        {
            supresMtx[y][x] = calcLocalSupression(x,y, dirMtx[y][x]);
        }
    }

    //write finished image to matrix and advance stage flag
    overWrtPixMtx(supresMtx);
    stage = nonMaxSupressed;
}

uint8_t ImgMtx::calcLocalSupression(int x, int y, uint8_t dir)
{
    //decodes direction of sobel edge and applies non-max suppression
    //by viewing the pixMtx

    //using safe version of get pixel as the coords should never be outside of the image for the origin
    uint8_t curPix = s_getPixel(x,y);
    switch(dir)
    {
    case 0:
        //up-down direction
        if(curPix < getPixel(x,y+1) || curPix < getPixel(x,y-1))
        {
            curPix = 0;
        }
        return curPix;

    case 1:
        //left-right direction
        if(curPix < getPixel(x+1,y) || curPix < getPixel(x-1,y))
        {
            curPix = 0;
        }
        return curPix;

    case 2:
        //upLeft-downRight direction
        if(curPix < getPixel(x+1,y+1) || curPix < getPixel(x-1,y-1))
        {
            curPix = 0;
        }
        return curPix;

    case 3:
        //upLeft-downRight direction
        if(curPix < getPixel(x-1,y+1) || curPix < getPixel(x+1,y-1))
        {
            curPix = 0;
        }
        return curPix;

    default:
       throw std::invalid_argument("ERROR: direction data at (" + to_string(x) + "," + to_string(y) + ") is invalid, direction code of: " + to_string(dir));
    }
}

void ImgMtx::edgeLink()
{
    //perform edge linking by checking surrounding pixels
    //produces a binary image with pixels set to 0 or 255

    if(stage != nonMaxSupressed)
    {
        cout << "WARNING: Edge linking is being performed out of order, image likely to be corrupted." << endl;
    }

    uint8_t ** edgeMtx = new uint8_t*[height];
    for(int i = 0; i < height; i++)
    {
        edgeMtx[i] = new uint8_t[width];
    }

    const int strongPixReq = 3; //strong pixels needed within kernel for a weak origin to pass
    const int strongThres = 110; //threshold for a strong pixel, 0-255
    const int linkKerLen = 3; //side length for edge linking kernel, every pixel inside the kernel is considered 'adjacent'

    int lowerKerCorner = -floor(linkKerLen / 2);
    int upperKerCorner = floor(linkKerLen / 2);

    for(int y = 0; y < height; y++)
    {
        for(int x = 0; x < width; x++)
        {
            //check if pixel is already strong, safe getPixel as should never be out of range
            if(s_getPixel(x,y) >= strongThres)
            {
                //if true then set pixel high
                edgeMtx[y][x] = 255;
            }
            else
            {
                //if not count nearby pixels that are above threshold
                int strongPixCount = 0;
                for(int j = lowerKerCorner; j <= upperKerCorner; j++)
                {
                    for(int i = lowerKerCorner; i <= upperKerCorner; i++)
                    {
                        if( getPixel(x+i, y+j) >= strongThres)
                        {
                            strongPixCount++;
                        }
                    }
                }

                //if count is equal or higher than requirement, set pixel high, if not set low
                if(strongPixCount >= strongPixReq)
                {
                    edgeMtx[y][x] = 255;
                }
                else
                {
                    edgeMtx[y][x] = 0;
                }
            }
        }
    }

    //overwrite image matrix and advance stage flag
    overWrtPixMtx(edgeMtx);
    stage = edgeLinked;
}

vector<boundingBox> ImgMtx::getBoundingBoxes()
{
    //finds and bounds all shapes connected by adjacent pixels
    if(stage != edgeLinked)
    {
        throw std::invalid_argument("ERROR: unable to perform bounding box finding on a non edge linked image.");
    }

    //create temporary clone of image matrix
    uint8_t ** bbMtx = new uint8_t*[height];
    for(int i = 0; i < height; i++)
    {
        bbMtx[i] = new uint8_t[width];
    }

    for(int j = 0; j < height; j++)
    {
        for(int i = 0; i < width; i++)
        {
            bbMtx[j][i] = s_getPixel(i,j);
        }
    }

    vector<boundingBox> bbList; //output vector for the collected bounding boxes

    for(int y = 0; y < height; y++)
    {
        for(int x = 0; x < width; x++)
        {
            if(getInpMtx(x,y, bbMtx) == 255)
            {
                bbList.push_back(boundShape(x,y,bbMtx));
            }
        }
    }

    //free temporary clone of image matrix
    for(int i = 0; i < height; i++)
    {
        delete[] bbMtx[i];
    }
    delete[] bbMtx;

    return bbList;
}

boundingBox ImgMtx::boundShape(int startX, int startY, uint8_t** bbMtx)
{
    //bound the shape by taking the high/low of the x/y collection of
    //pixels found to be adjacent to each other in the shape

    //create starting pixel
    bbPixel startPix;
    startPix.x = startX;
    startPix.y = startY;

    const int boundingKerLen = 5; //defines length of kernel, everything inside this is considered 'adjacent' to the current pixel
    vector<bbPixel> unCheckedPixels; //collection of pixels yet to be searched for adjacent positive pixels
    unCheckedPixels.push_back(startPix);

    //set starting values for low/high x/y values for shape
    uint16_t lowY,lowX,highY,highX;
    lowY = 0xFFFF;
    lowX = 0xFFFF;
    highY = 0;
    highX = 0;

    int lowerKerCorner = -floor(boundingKerLen / 2);
    int upperKerCorner = floor(boundingKerLen / 2);

    bbPixel curPix;

    while(unCheckedPixels.size() != 0)
    {
        //get the current search pixel (last element in vector), and clear pixel to prevent double checking
        curPix = unCheckedPixels.at(unCheckedPixels.size() - 1);
        unCheckedPixels.pop_back();
        //this index is safe as a pixel cannot be created without a valid coord
        bbMtx[curPix.y][curPix.x] = 0;

        //check for coord high/low
        if(curPix.y > highY)
        {highY = curPix.y;}

        if(curPix.y < lowY)
        {lowY = curPix.y;}

        if(curPix.x > highX)
        {highX = curPix.x;}

        if(curPix.x < lowX)
        {lowX = curPix.x;}

        for(int j = lowerKerCorner; j <= upperKerCorner; j++)
        {
            for(int i = lowerKerCorner; i <= upperKerCorner; i++)
            {
                if( getInpMtx(i+curPix.x, j+curPix.y, bbMtx) == 255 )
                {
                    //if new pixel in shape found, add pixel to list to check later
                    bbPixel newPix;
                    newPix.x = i+curPix.x;
                    newPix.y = j+curPix.y;
                    unCheckedPixels.push_back(newPix);
                    //set found pixel as blank, will never make an acsess out of range as out of range pixels are always 0
                    bbMtx[newPix.y][newPix.x] = 0;
                }
            }
        }
    }

    //construct final bounding box struct
    boundingBox foundBox;
    foundBox.x1 = lowX;
    foundBox.y1 = lowY;
    foundBox.x2 = highX;
    foundBox.y2 = highY;

    return foundBox;
}

uint8_t ImgMtx::getInpMtx(int x, int y, uint8_t** inpMtx)
{
    //gets the pixel at input coords from inpMtx
    //!assumes range is same as pixMtx, will return 0 if outside of range
    if( x < 0 || y < 0 || x >= width || y >= height )
    {return 0;}

    return inpMtx[y][x];
}

int ImgMtx::writeImg(const char * fileNmOut)
{
    //write data from pixMtx into a char buffer to be written to file by stb lib function

	// int stbi_write_jpg(char const *filename, int w, int h, int comp, const void *data, int quality);
	//create pointer to memory buffer, single layer image internally so buffer byte size is w*h
	if(pixMtx == NULL)
    {
        throw std::invalid_argument("ERROR: no image data loaded.");
    }

	uint8_t * dataOut = new uint8_t[height*width];

	unsigned long idx = 0;
	for(int j = 0; j < height; j++)
	{
		for(int i = 0; i < width; i++)
		{
			dataOut[idx++] = pixMtx[j][i];
		}
	}

	int wrtCode = stbi_write_jpg(fileNmOut, width, height, BYTES_PER_PIX, dataOut, QUALITY_SETTING);

	//clear buffer from memory
	stbi_image_free(dataOut);

	//return writer status flag
	return wrtCode;
}
