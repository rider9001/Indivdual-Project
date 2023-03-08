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

#include "GPIOPort.cpp"

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
    //setup processing thread
    std::thread imgProcessingThread(imageProcessingProc);

    GPIOPort IRSensorPort("14", GPIO_INPUT);

    bool motionDetectedLast = false;

    for(;;)
    {
        std::this_thread::sleep_until( std::chrono::system_clock::now() + std::chrono::milliseconds(100));
        
        cout << "IR sensor value " << IRSensorPort.getval_gpio() << endl;
        if(motionDetectedLast)
        {
            if(IRSensorPort.getval_gpio() == 1)
            {motionDetectedLast = false;} //hold in lock state until proximity sensor goes high
        } else {
            if(IRSensorPort.getval_gpio() == 0 && !motionDetectedLast) //low is a detection of proximity
            {
                cout << "Motion detected, starting capture" << endl;
                runHighSpeedCapture();
                motionDetectedLast = true;
            } else {
                motionDetectedLast = false;
            }
        }
    }

	return 0;
}