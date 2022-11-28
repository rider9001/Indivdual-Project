/*
	Class intended to be passed the filename of a jpg file to
	read with stb_image then convert to grayscale
	Grayscale formula: 0.3R + 0.6G + 0.1B = Y
	
	Reads image file into memory with constructor
*/

#include <vector>

using namespace std;

class GrayImgMtx {
	private:
		vector< vector<unsigned char> > pixMtx;
		int width, height;
		const char * originFilename;
		#define BYTES_PER_PIX 1
		#define QUALITY_SETTING 90 //100 is max
		
		unsigned char grayscalePixel(unsigned char R, unsigned char G, unsigned char B);
		
	public:
		GrayImgMtx(const char * filename);
		int writeImg(const char * filename);
};

GrayImgMtx::GrayImgMtx(const char * filename)
{
	//read file and get metadata
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
	cout << originFilename << " is " << pixMtx[0].size() << " by " << pixMtx.size() << " pixels." << endl;
	
	for(int j = 0; j < height; j++)
	{
		for(int i = 0; i < width; i++)
		{
			int idx = j + i;
			pixMtx.at(j).at(i) = grayscalePixel(data[idx], data[idx+1], data[idx+2]);
			//cout << (int)pixMtx.at(j).at(i) << endl;
		}
	}
	
	//delete stb image data
	stbi_image_free(data);
}

unsigned char GrayImgMtx::grayscalePixel(unsigned char R, unsigned char G, unsigned char B)
{
	R = ( (R<<1) + R ) / 10; // 0.3 * R
	G = ( (G<<2) + (G<<1) ) / 10; // 0.6 * G
	B = B / 10; // 0.1 * B
	
	return R + G + B;
}

int GrayImgMtx::writeImg(const char * fileNmOut)
{
	// int stbi_write_jpg(char const *filename, int w, int h, int comp, const void *data, int quality);	
	//create pointer to memory buffer, single layer image internally so buffer byte size is w*h
	unsigned char * dataOut = new unsigned char[height*width];
	
	unsigned int idx = 0;
	for(int j = 0; j < height; j++)
	{
		for(int i = 0; i < width; i++)
		{
			dataOut[idx++] = pixMtx.at(j).at(i);
		}
	}

	int wrtCode = stbi_write_png(fileNmOut, width, height, BYTES_PER_PIX, dataOut, width);
	
	//clear buffer from memory
	stbi_image_free(dataOut); 
	
	//return writer status flag
	return wrtCode;
}