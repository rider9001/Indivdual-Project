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
		#define QUALITY_SETTING 100 //100 is max
		
	public:
		GrayImgMtx(const char * filename);
		void writeImg(const char * filename);
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
	//cout << "Image is " << width << " by " << height << " pixels." << endl;
	
	//delete stb image data
	stbi_image_free(data);
}

void writeImg(const char * filename)
{
	throw std::domain_error("Unimplemented function: writeImg");
}