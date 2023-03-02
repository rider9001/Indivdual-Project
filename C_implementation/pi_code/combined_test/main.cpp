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
#include <mutex>

using namespace std;

#include "boundingBoxStructs.h"

#include "ImgMtx.cpp"
#include "boxFilter.cpp"

#include "imageSpeedCalcs.cpp"
#include "motionDetectFuncs.cpp"

#include "atomicFIFO.cpp"

atomicFIFO imgChainProcessingQueue;

void imageProcessingProc()
{
    while(true)
    {
        if( imgChainProcessingQueue.empty() )
        {
            std::this_thread::sleep_until( std::chrono::system_clock::now() + std::chrono::seconds(5) );
        } else {
            std::string analysisDir = imgChainProcessingQueue.pop();

            cout << "USER:Starting analysis on directory: " << analysisDir << endl;

            analyseCamChain(analysisDir);

            cout << "USER:Analysis done on: " << analysisDir << endl;
        }
    }
}

void runHighSpeedCapture()
{
    int code = system("bash HighSpeedCamRun.sh > /dev/null");

    if(code != 0)
    {throw std::invalid_argument("ERROR: bash High speed Cam failed.");}

    regex DTFileRegex("\\S+DT:\\S+$");

	string tempDirPath = "temp";
	vector<string> jpgFileNames;

    string dateTimeStr = "";
    for (const auto & entry : std::filesystem::directory_iterator(tempDirPath))
	{
		if( regex_match(entry.path().c_str(), DTFileRegex, regex_constants::match_default) )
		{
            string tempStr = entry.path().string();
            bool foundStart = false;
            const char DTStartChar = ':';
            for(int strIdx = 0; strIdx < tempStr.length(); strIdx++)
            {
                if( foundStart )
                {
                    dateTimeStr += tempStr[strIdx];
                } else if( tempStr[strIdx] == DTStartChar ) {
                    foundStart = true;
                }
            }
            break;
        }
	}

    cout << "USER:Date time value: " << dateTimeStr << endl;

    system( ("mkdir " + dateTimeStr).c_str() );
    system( ("mv -v " + tempDirPath + "/* " + dateTimeStr).c_str() );

    imgChainProcessingQueue.push(dateTimeStr);
}

int main()
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
        cout << "USER:Camera completed in " << camTime.count() << "s" << endl;
    }

    ImgMtx * curImg = new ImgMtx(capturePath.c_str());
    curImg->gaussBlur();
    curImg->SobelFil();

    ImgMtx * prevImg = nullptr;

    start = std::chrono::system_clock::now();
    end = std::chrono::system_clock::now();

    //setup processing thread
    std::thread imgProcessingThread(imageProcessingProc);

    bool motionDetectedLast = false;
    unsigned int motionDetectCount = 0;
    #define MAX_DETECTION_FRAMES_BEFORE_ESCAPE 5

    while(true)
    {
        std::this_thread::sleep_until( std::chrono::system_clock::now() + std::chrono::seconds(4) - (end - start) );
        start = std::chrono::system_clock::now();

        code = system("bash takeLowResStill.sh > /dev/null");
        if(code != 0)
        {
            throw std::invalid_argument("Low resolution camera capture failed.");
        } 

        //if motion was detected on the last test, do not advance baseline to the previous motion image
        //this prevents double captures, as otherwise the system would detect the appearance of new motion
        //then the disaperance of the motion
        if(motionDetectedLast and motionDetectCount < MAX_DETECTION_FRAMES_BEFORE_ESCAPE)
        {
            //cout << "USER:No image advancement" << endl;

            delete curImg;
            curImg = new ImgMtx(capturePath.c_str());
        } else {
            motionDetectCount = 0;
            //cout << "USER:Image advancing" << endl;

            if(prevImg != nullptr)
            {
                delete prevImg;
            }

            prevImg = curImg;
            curImg = new ImgMtx(capturePath.c_str());
        }

        curImg->gaussBlur();
        curImg->SobelFil();

        ImgMtx * motionImg = imageSubtract(curImg, prevImg);
        boundingBox motionMask = detectMotion(motionImg);

        delete motionImg;

        if(motionMask.x1 == UINT16_MAX)
        {
            cout << "USER:Insignificant motion detected" << endl;
            motionDetectedLast = false;
        } else {
            cout << "USER:Motion detected: (" << motionMask.x1 << "," << motionMask.y1 << ") -> (" <<  motionMask.x2 << "," << motionMask.y2 << ")" << endl;
            motionDetectedLast = true;
            motionDetectCount++;

            system( ("rm " + capturePath).c_str() );

            runHighSpeedCapture();
        }

        cout << "--------------------" << endl << endl;
        end = std::chrono::system_clock::now();
    }

	return 0;
}