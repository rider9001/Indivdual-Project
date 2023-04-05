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

#include "pauseCtrl.cpp"
#include "imageSpeedCalcs.cpp"

#include "GPIOPort.cpp"

#include "atomicFIFO.cpp"

atomicFIFO imgChainProcessingQueue;
pauseCtrl workerThreadPause = pauseCtrl();

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

            while(workerThreadPause.isPaused())
            {
                std::this_thread::sleep_until( std::chrono::system_clock::now() + std::chrono::milliseconds(250) );
            }
            analyseCamChain(analysisDir, workerThreadPause);

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

int main(int argc, char ** argv)
{
    if(argc != 2)
    {
        cout << "Need directory" << endl;
        return 1;
    }

    string analysisDir = argv[1];

    analyseCamChain(analysisDir, workerThreadPause);

	return 0;
}