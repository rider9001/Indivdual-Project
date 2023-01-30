#include <stdlib.h>
#include <cstdio>
#include <iostream>
#include <stdexcept>
#include <chrono>
#include <ctime>
#include <filesystem>
#include <string>
#include <regex>
#include <vector>

using namespace std;

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include "filterStages.h"
#include "boundingBoxStructs.h"

#include "ImgMtx.cpp"
#include "boxFilter.cpp"

int main(int argc, char *argv[])
{
	regex jpgImgRegex("\\S+.jpg$");

	string path = "temp/";
	vector<string> jpgFiles;

    for (const auto & entry : std::filesystem::directory_iterator(path))
	{
		if( regex_match(entry.path().c_str(), jpgImgRegex, regex_constants::match_default) )
		{
			std::cout << entry.path().c_str() << " is a jpg file" << endl;
            jpgFiles.push_back(entry.path().c_str());
		} else {
			std::cout << entry.path().c_str() << " is not a jpg file" << endl;
		}
	}

    for(int i = 0; i  < jpgFiles.size(); i++)
    {
        cout << jpgFiles.at(i) << endl;
    }

	/*
	auto start = std::chrono::system_clock::now();
    ImgMtx testImg(filename);
	auto end = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsed_seconds = end-start;
	std::chrono::duration<double> totalTime = elapsed_seconds;
	start = std::chrono::system_clock::now();

	const char * outFileName = "ImgOut/GrayscaleOutput.jpg";
	start = std::chrono::system_clock::now();

	cout << "Writing file: " << outFileName << endl;
	int code = testImg.writeImg(outFileName);
	end = std::chrono::system_clock::now();

    if(filtered.size() == 0)
    {
        cout << "No passing boxes found." << endl;
    }
    else
    {
         cout << "(x1,y1) -> (x2,y2)" << endl;
        for(unsigned int i = 0; i < filtered.size(); i++)
        {
            boundingBox curBox = filtered.at(i);
            cout << "Box " << (i+1) << ": (" << curBox.x1 << "," << curBox.y1 << ") -> (" << curBox.x2 << "," << curBox.y2 << "), Area: " << (curBox.y2 - curBox.y1) * (curBox.x2 - curBox.x1) <<endl;

            avgBox.x1 += curBox.x1 / filtered.size();
            avgBox.x2 += curBox.x2 / filtered.size();
            avgBox.y1 += curBox.y1 / filtered.size();
            avgBox.y2 += curBox.y2 / filtered.size();
        }

        cout << "Average area: " << (avgBox.y2 - avgBox.y1) * (avgBox.x2 - avgBox.x1) << endl;
        cout << "Average width: " << avgBox.x2 - avgBox.x1 << endl;
        cout << "Average height: " << avgBox.y2 - avgBox.y1 << endl;
        cout << "Average box: (" << avgBox.x1 << "," << avgBox.y1 << ") -> (" << avgBox.x2 << "," << avgBox.y2 << ")" << endl;
    }

    cout << "----All tests complete----" << endl;
    cout << "Tests complete in: " << totalTime.count() << "s" << endl;
	*/

	return 0;
}