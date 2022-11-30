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

    if(imageLoaded)
    {
        for(int i = 0; i < height; i++)
        {
            delete[] pixMtx[i];
        }
        delete[] pixMtx;
    }

    if(maxSupressed)
    {
        for(int i = 0; i < height; i++)
        {
            delete[] angMtx[i];
        }
        delete[] angMtx;
    }
}

ImgMtx::ImgMtx()
{
    imageLoaded = false;
    pixMtx = NULL;
    width = -1;
    height = -1;
    originFilename = "NO FILE LOADED";
}

ImgMtx::ImgMtx(uint8_t ** mtxPtr, int widthIn, int heightIn)
{
    originFilename = "Name not given";
    pixMtx = mtxPtr;
    width = widthIn;
    height = heightIn;
    imageLoaded = true;
}

ImgMtx::ImgMtx(uint8_t ** mtxPtr, int widthIn, int heightIn, const char * fileNmIn)
{
    originFilename = fileNmIn;
    pixMtx = mtxPtr;
    width = widthIn;
    height = heightIn;
    imageLoaded = true;
}

ImgMtx::ImgMtx(const char * filename)
{
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

	imageLoaded = true;

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

void ImgMtx::overWrtPixMtx(uint8_t ** newMtx)
{
    //deletes old matrix pointers and replaces main pointer with new matrix pointer

    for(int i = 0; i < height; i++)
    {
        delete[] pixMtx[i];
    }
    delete pixMtx;

    pixMtx = newMtx;
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
    return imageLoaded;
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
    if(!imageLoaded)
    {
        throw std::invalid_argument("ERROR: no image data loaded.");
    }

    if(stage != Grayscale)
    {
        cout << "WARNING: Filter stage out of order, performing gaussian filter on a stage other than grayscale." << endl;
    }

    //define coefficient list for kernel, will probably make this dynamic in future
    #define coeffListLen 11
    const uint8_t coeffList[coeffListLen] = {1,10,45,120,210,252,210,120,45,10,1};
    #define divideShift 20 //divide by 2^20, sum of all coefficients squared (N>>divideShift)

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

    //define sobel convolution filters
    const int fil_ver[9] = {-1,0,1,-2,0,2,-1,0,1};
    const int fil_hor[9] = {-1,-2,-1,0,0,0,1,2,1};
}

int ImgMtx::writeImg(const char * fileNmOut)
{
    //write data from pixMtx into a char buffer to be written to file by stb lib function

	// int stbi_write_jpg(char const *filename, int w, int h, int comp, const void *data, int quality);
	//create pointer to memory buffer, single layer image internally so buffer byte size is w*h
	if(!imageLoaded)
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
