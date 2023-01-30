#include <cstdio>
#include <iostream>
#include <string>
#include <stdexcept>
#include <chrono>
#include <ctime>  

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
	
	//test read-write, delete for speed
	int x,y,n;
	unsigned char * tstData = stbi_load(filename, &x, &y, &n, 0);
	stbi_write_jpg("out/TestOutput.jpg", x, y, n, tstData, 90);
	stbi_image_free(tstData);	
	
	auto start = std::chrono::system_clock::now();
	GrayImgMtx tstImg1(filename);
	auto end = std::chrono::system_clock::now();
	
	std::chrono::duration<double> elapsed_seconds = end-start;
	
	cout << "File " << filename << " read complete in " << elapsed_seconds.count() << "s" << endl;
	
	const char * outFileName = "out/GrayscaleOutput.jpg";
	start = std::chrono::system_clock::now();
	
	cout << "Writing file: " << outFileName << endl;
	int code = tstImg1.writeImg(outFileName);
	
	
	end = std::chrono::system_clock::now();
	
	elapsed_seconds = end-start;
	
	cout << "File write complete in " << elapsed_seconds.count() << "s" << endl;
	
	if(code != 0)
	{
		cout << "File write sucsess" << endl;
	}
	else
	{
		cout << "File write failure" << endl;
	}
		
	return 0;
}