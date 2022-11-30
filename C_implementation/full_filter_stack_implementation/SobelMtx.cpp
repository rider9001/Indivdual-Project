/*
    Class intended to compute and store the sobel and non-max suppressed sobel filter of an input image file or matrix
    Can perform a non maximum suppression to return another SobelMtx object
    Inherits all reads/writes from ImgMtx
*/


class SobelMtx: public ImgMtx
{
    protected:
        uint8_t ** angMtx;
        bool maxSupressed;

        void SobelFilImgMtx();

        uint8_t getAng(int x, int y);

    public:
        SobelMtx(const char * fileNmIn);
		SobelMtx(unsigned char ** mtxPtr, int widthIn, int heightIn);
		SobelMtx(unsigned char ** mtxPtr, int widthIn, int heightIn, const char * fileNmIn);
		~SobelMtx();

        uint8_t s_getAng(int x, int y);
        bool getMaxSupressed();
};

SobelMtx::~SobelMtx()
{
    //cleans angMtx and triggers parent destructor
    //only deletes matrix if data loaded

    if(maxSupressed)
    {
        for(int i = 0; i < height; i++)
        {
            delete[] angMtx[i];
        }
        delete[] angMtx;
    }
}

SobelMtx::SobelMtx(const char * fileNmIn)
{
    ImgMtx(fileNmIn);
    SobelFilImgMtx();
}

SobelMtx::SobelMtx(unsigned char ** mtxPtr, int widthIn, int heightIn)
{
    ImgMtx(mtxPtr, widthIn, heightIn);
    SobelFilImgMtx();
}

SobelMtx::SobelMtx(unsigned char ** mtxPtr, int widthIn, int heightIn, const char * fileNmIn)
{
    ImgMtx(mtxPtr, widthIn, heightIn, fileNmIn);
    SobelFilImgMtx();
}
