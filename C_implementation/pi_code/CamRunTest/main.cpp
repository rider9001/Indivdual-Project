#include <stdlib.h>
#include <cstdio>
#include <iostream>
#include <stdexcept>
#include <chrono>
#include <ctime>
#include <filesystem>
#include <string>
#include <vector>

using namespace std;

#include "src/filterStages.h"
#include "src/boundingBoxStructs.h"

#include "src/ImgMtx.cpp"
#include "src/boxFilter.cpp"

#include "src/imageSpeedCalcs.cpp"

int main(int argc, char *argv[])
{
	regex DTFileRegex("\\S+DT:\\S+$");

	string tempDirPath = "temp";
	vector<string> jpgFileNames;

    int code = system("bash HighSpeedCamRun.sh");

    if(code != 0)
    {throw std::invalid_argument("ERROR: bash High speed Cam failed.");}

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

    cout << "Date time value: " << dateTimeStr << endl;

    system( ("mkdir " + dateTimeStr).c_str() );
    system( ("mv -v " + tempDirPath + "/* " + dateTimeStr).c_str() );

    analyseCamChain(dateTimeStr);

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