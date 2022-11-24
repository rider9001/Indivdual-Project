#include <cstdio>
#include <iostream>
#include <string>
#include <stdexcept>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include "GrayImgMtx.h"

using namespace std;

int main(int argc, char *argv[])
{
	if(argc < 2)
	{
		cout << "usage: filename";
		return 1;
	}
	
	const char * filename = argv[1];
	
	GrayImgMtx tstImg1(filename);
}