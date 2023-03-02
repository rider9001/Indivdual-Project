#ifndef IMGMTX_HPP_INCLUDED
#define IMGMTX_HPP_INCLUDED

#include <math.h>
#include <vector>
#include <string>

#include "filterStages.h"
#include "boundingBoxStructs.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

class ImgMtx {
	private:
	    //enum made to track the stage in the filter stack object is on
	    filterStage stage = Grayscale;

	    //baseline grayscaling and gaussian filter vars
		uint8_t ** pixMtx = NULL;
		int width, height;
		string originFilename;
		#define BYTES_PER_PIX 1
		#define QUALITY_SETTING 100 //100 is max quality

		//sobel filter and suppression vars
        uint8_t ** dirMtx = NULL;

        //utility methods
        void overWrtPixMtx(uint8_t **);
        void overWrtDirMtx(uint8_t **);
        uint8_t getInpMtx(int x, int y, uint8_t** inpMtx);

        //greyscale methods
		uint8_t grayscalePixel(uint8_t R, uint8_t G, uint8_t B);
		uint8_t getPixel(int x, int y);

		//sobel/max supression stage methods
		uint8_t getAng(int x, int y);
		uint8_t aproxDir(int16_t Ver, int16_t Hor);

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
		string getSourceFilename();
		filterStage getFilterStage();

		//gaussian filter methods
        void gaussBlur();

		//sobel/max supression stage methods
		uint8_t s_getAng(int x, int y);
        bool getMaxSupressed();
        void SobelFil();
        void nonMaxSupress();
        uint8_t calcLocalSupression(int x, int y, uint8_t dir);

        //edge linking methods
        void edgeLink();

        //bounding box finding methods
        vector<boundingBox> getBoundingBoxes();
        boundingBox boundShape(int startX, int startY, uint8_t** mtx);

		//perform full filter process
		void fullFilter();
};

#endif // IMGMTX_H_INCLUDED
