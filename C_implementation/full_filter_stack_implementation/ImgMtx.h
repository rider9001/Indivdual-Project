#ifndef IMGMTX_H_INCLUDED
#define IMGMTX_H_INCLUDED

#include <math.h>

class ImgMtx {
	private:
	    //enum made to track the stage in the filter stack object is on
	    filterStage stage = Grayscale;

	    //baseline grayscaling and gaussian filter vars
		uint8_t ** pixMtx;
		int width, height;
		const char * originFilename;
		bool imageLoaded;
		#define BYTES_PER_PIX 1
		#define QUALITY_SETTING 100 //100 is max quality

		//sobel filter and suppression vars
        uint8_t ** angMtx;
        bool maxSupressed;

        //utility methods
        void overWrtPixMtx(uint8_t ** newMtx);

        //greyscale methods
		uint8_t grayscalePixel(uint8_t R, uint8_t G, uint8_t B);
		uint8_t getPixel(int x, int y);

		//sobel/max supression stage methods
		uint8_t getAng(int x, int y);

	public:
	    //can be constructed with a file read, as blank or with a pre created matrix
	    //should only really be constructed using a file read though, other constructors are likely prone to
	    //dead pointer errors if pointers are sourced from other ImgMtx objects
		ImgMtx(const char * filename);
		ImgMtx();
		ImgMtx(uint8_t ** mtxPtr, int widthIn, int heightIn);
		ImgMtx(uint8_t ** mtxPtr, int widthIn, int heightIn, const char * fileNmIn);
		~ImgMtx();

		//utility methods
		filterStage getStage();

		//greyscale methods
		int writeImg(const char * filename);
		uint8_t s_getPixel(int x, int y);
		bool getImLoaded();
		int getWidth();
		int getHeight();
		const char * getSourceFilename();

		//gaussian filter methods
        void gaussBlur();

		//sobel/max supression stage methods
		uint8_t s_getAng(int x, int y);
        bool getMaxSupressed();
        void SobelFil();
};

#endif // IMGMTX_H_INCLUDED
