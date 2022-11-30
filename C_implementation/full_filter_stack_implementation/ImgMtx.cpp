/*
	Class intended to store all functions and information relating to image transforms and
	image reading/writing
*/

#include <vector>
#include <math.h>

using namespace std;

class ImgMtx {
	private:
		vector< vector<unsigned char> > pixMtx;
		int width, height;
		const char * originFilename;
		bool imageLoaded;
		#define BYTES_PER_PIX 1
		#define QUALITY_SETTING 100 //100 is max quality

		unsigned char grayscalePixel(unsigned char R, unsigned char G, unsigned char B);
		unsigned char getPixel(int x, int y);

	public:
	    //can be constructed with a file read, as blank or with a pre created matrix
		ImgMtx(const char * filename);
		ImgMtx();
		ImgMtx(vector< vector<unsigned char> >);
		ImgMtx(vector< vector<unsigned char> >, const char *);

		int writeImg(const char * filename);
		unsigned char s_getPixel(int x, int y);
		ImgMtx gaussBlur();

		bool getImLoaded();
		int getWidth();
		int getHeight();
		const char * getSourceFilename();
};

ImgMtx::ImgMtx()
{
    imageLoaded = false;
    width = -1;
    height = -1;
    originFilename = "NO FILE LOADED";
}

ImgMtx::ImgMtx(vector< vector<unsigned char> > inpMtx)
{
    originFilename = "Name not given";
    pixMtx = inpMtx;
    width = inpMtx[0].size();
    height = inpMtx.size();
    imageLoaded = true;
}

ImgMtx::ImgMtx(vector< vector<unsigned char> > inpMtx, const char * setName)
{
    originFilename = setName;
    pixMtx = inpMtx;
    width = inpMtx[0].size();
    height = inpMtx.size();
    imageLoaded = true;
}

ImgMtx::ImgMtx(const char * filename)
{
	//read file and get meta-data
	int x,y,n;
	unsigned char *data = stbi_load(filename, &x, &y, &n, 0);

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

	pixMtx = vector< vector<unsigned char> >(height);
	for(int i = 0; i < height; i++)
	{
		pixMtx.at(i) = vector<unsigned char>(width);
	}
	//img coords are (row, col)
	//cout << originFilename << " is " << pixMtx[0].size() << " by " << pixMtx.size() << " pixels." << endl;

	unsigned long pixIdx;
	for(int j = 0; j < height; j++)
	{
		pixIdx = j * width * 3;
		for(int i = 0; i < width; i++)
		{
			pixMtx.at(j).at(i) = grayscalePixel(data[pixIdx], data[pixIdx+1], data[pixIdx+2]);
			//cout << (int)pixMtx.at(j).at(i) << " from index: " << pixIdx << endl;
			pixIdx += 3;
		}
	}

	//delete stb image data
	stbi_image_free(data);
}

unsigned char ImgMtx::s_getPixel(int x, int y)
{
    //public facing version of pixel get, allowed to throw exception if out of range
    return pixMtx.at(y).at(x);
}

unsigned char ImgMtx::getPixel(int x, int y)
{
    //unsafe internal version of pixel get, returns 0 on out of bounds coordiante
    if( x < 0 || y < 0 || x >= width || y >= height )
    {return 0;}

    return pixMtx.at(y).at(x);
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

unsigned char ImgMtx::grayscalePixel(unsigned char R, unsigned char G, unsigned char B)
{
	R = ( (R<<1) + R ) / 10; // 0.3 * R
	G = ( (G<<2) + (G<<1) ) / 10; // 0.6 * G
	B = B / 10; // 0.1 * B

	return R + G + B;
}

ImgMtx ImgMtx::gaussBlur()
{
    //returns a new ImgMtx obj that contains the gaussian blur of the current objects image
    if(!imageLoaded)
    {
        throw std::invalid_argument("ERROR: no image data loaded.");
    }

    //define coefficient list for kernel, will probably make this dynamic in future
    #define coeffListLen 11
    uint8_t coeffList[coeffListLen] = {1,10,45,120,210,252,210,120,45,10,1};
    #define divideShift 20 //divide by 2^20, sum of all coefficients squared (N>>divideShift)

    //create output matrix
    vector< vector<unsigned char> > gaussMtx = vector< vector<unsigned char> >(height);
    for(int i = 0; i < height; i++)
    {
        gaussMtx.at(i) = vector<unsigned char>(width);
    }

    //create upper and lower offsets to apply convolution filter to matrix
    int lowerKerCorner = -floor(coeffListLen / 2);
    int upperKerCorner = floor(coeffListLen / 2);

    cout << "Gauss loop:";
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
                    unsigned char pixVal = getPixel( i+x , j+y );
                    //skip calculation if pixVal is zero
                    if(pixVal != 0)
                    {
                        //calculate blur value, array offset is used to return j/i to 0:coeffListLen range from lowerKerCorner:upperKerCorner
                        pixTotal += pixVal * coeffList[j+upperKerCorner] * coeffList[i+upperKerCorner];
                    }
                }
            }
            gaussMtx.at(y).at(x) = (unsigned char)(pixTotal >> divideShift);
        }
    }
    cout << " finished" << endl;

    return ImgMtx(gaussMtx);
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

	unsigned char * dataOut = new unsigned char[height*width];

	unsigned long idx = 0;
	for(int j = 0; j < height; j++)
	{
		for(int i = 0; i < width; i++)
		{
			dataOut[idx++] = pixMtx.at(j).at(i);
		}
	}

	int wrtCode = stbi_write_jpg(fileNmOut, width, height, BYTES_PER_PIX, dataOut, QUALITY_SETTING);

	//clear buffer from memory
	stbi_image_free(dataOut);

	//return writer status flag
	return wrtCode;
}
