#include <stdlib.h>
#include <mutex>
#include <cstdio>
#include <iostream>
#include <stdexcept>
#include <filesystem>
#include <string>
#include <sstream>
#include <vector>
#include <chrono>
#include <thread>

using namespace std;

#include "src/boundingBoxStructs.h"

#include "src/ImgMtx.cpp"
#include "src/boxFilter.cpp"

#include "src/imageSpeedCalcs.cpp"
#include "src/motionDetectFuncs.cpp"

int main(int argc, char *argv[])
{
	string capturePath = "temp/motion.jpg";

    auto start = std::chrono::system_clock::now();
    int code = system("bash takeLowResStill.sh > /dev/null");
    auto end = std::chrono::system_clock::now();

    std::chrono::duration<double> camTime = end-start;

    if(code != 0)
    {
        throw std::invalid_argument("Low resolution camera capture failed.");
    } else {
        cout << "Camera completed in " << camTime.count() << "s" << endl;
    }

    ImgMtx * curImg = new ImgMtx(capturePath.c_str());
    curImg->gaussBlur();
    curImg->SobelFil();

    ImgMtx * prevImg = nullptr;

    start = std::chrono::system_clock::now();
    end = std::chrono::system_clock::now();

    unsigned int motionImgCount = 0;

    while(true)
    {
        std::this_thread::sleep_until( std::chrono::system_clock::now() + std::chrono::seconds(3) - (end - start) );
        start = std::chrono::system_clock::now();

        code = system("bash takeLowResStill.sh > /dev/null");
        if(code != 0)
        {
            throw std::invalid_argument("Low resolution camera capture failed.");
        } 

        if(prevImg != nullptr)
        {
            delete prevImg;
        }

        prevImg = curImg;
        curImg = new ImgMtx(capturePath.c_str());

        curImg->gaussBlur();
        curImg->SobelFil();

        ImgMtx * motionImg = imageSubtract(curImg, prevImg);
        boundingBox motionMask = HFTstDetectMotion(motionImg);

        delete motionImg;

        if(motionMask.x1 == UINT16_MAX)
        {
            cout << "Insignificant motion detected" << endl;
        } else {
            cout << "Motion detected: (" << motionMask.x1 << "," << motionMask.y1 << ") -> (" <<  motionMask.x2 << "," << motionMask.y2 << ")" << endl;

            ImgMtx * originImg = new ImgMtx(capturePath.c_str());

            ImgMtx * motionMaskImg = MaskImg(originImg, motionMask);
            ostringstream motionFileNm;
            motionFileNm << "motion" << motionImgCount++ << ".jpg";
            motionMaskImg->writeImg(motionFileNm.str().c_str());

            delete originImg;
        }

        cout << "--------------------" << endl << endl;
        end = std::chrono::system_clock::now();
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