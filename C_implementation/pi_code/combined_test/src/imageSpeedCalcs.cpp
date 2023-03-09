#include <chrono>
#include <ctime>
#include <vector>
#include <string>
#include <regex>

#include "imageSpeedCalcs.hpp"

void analyseCamChain(string directory)
{
    auto startT = std::chrono::system_clock::now();

    //note the directory is assumed to already exist below the pwd
    //the dir name is the date/time of the incident

    //empty file flags to be written when directories start and finish processing
    #define PROCESSING_FLAG_FILE "processing"
    #define COMPLETED_FLAG_FILE "completed"

    system( ("touch " + directory + "/" + PROCESSING_FLAG_FILE).c_str() );

    regex jpgImgRegex("\\S+.jpg$");//regex that matches '*.jpg' filenames

	vector<string> jpgFileNames;
    for (const auto & entry : std::filesystem::directory_iterator(directory))
	{
		if( regex_match(entry.path().c_str(), jpgImgRegex, regex_constants::match_default) )
		{ jpgFileNames.push_back( entry.path().c_str()); }
	}

    std::sort(jpgFileNames.begin(), jpgFileNames.end());//use basic sort to get images in order

    camChain curCamChain(jpgFileNames); //create image chain for analysis

    vector<imageBBresults> imgResults(curCamChain.size());
    for(unsigned int i = 0; i < curCamChain.size(); i++)
    {
        imgResults.at(i) = calcAvgBox( curCamChain.at(i) );
    }

    velDeltaResults velResults = evalImageResults(imgResults);

    auto endT = std::chrono::system_clock::now();

    ofstream statsFile;
    statsFile.open(directory + "/" + STATS_FILENM);

    statsFile << "Incident occoured at: " << directory << endl;
    statsFile << "Total images captured: " << curCamChain.size() << endl;
    statsFile << endl;

    for(unsigned int i = 0; i < imgResults.size(); i++)
    {
        imageBBresults curRes = imgResults.at(i);

        statsFile << "------ Image " << (i+1) << " stats ------" << endl;
        statsFile << "Filename: " << curRes.img->getSourceFilename() << endl;
        statsFile << "Plate matches found: " << curRes.foundBoxCount << endl;
        statsFile << "Average box: (" << curRes.avgBox.x1 << "," << curRes.avgBox.y1 << ") -> (" << curRes.avgBox.x2 << "," << curRes.avgBox.y2 << ")" << endl;
        statsFile << "Average box size: " << calcBoxArea(curRes.avgBox) << endl;

        statsFile << endl;
    }

    statsFile << "Average width delta: " << velResults.avgWidthDelta << endl;
    statsFile << "Average velocity delta: " << velResults.avgVelDelta << endl;
    statsFile << "Useful images in image chain: " << velResults.usefulImageCount << endl;

    std::chrono::duration<double> totalTime = endT - startT;
    statsFile << "Time to complete analysis: " << totalTime.count() << "s" << endl;

    statsFile.close();

    //delete processing flag and write completed flag
    system( ("rm " + directory + "/" + PROCESSING_FLAG_FILE).c_str() );
    system( ("touch " + directory + "/" + COMPLETED_FLAG_FILE).c_str() );
}

imageBBresults calcAvgBox(ImgMtx * img)
{
    //assumes that the image has had no processing done on it whatsoever, aside from greyscaling performed on image read

    img->fullFilter();
    vector<boundingBox> validBoxes = boxFilter(img->getBoundingBoxes(), img->getWidth(), img->getHeight());
    int num = validBoxes.size();

    unsigned int x1Tot = 0, x2Tot = 0, y1Tot = 0, y2Tot = 0;
    for(unsigned int i = 0; i < validBoxes.size(); i++)
    {
        x1Tot += validBoxes.at(i).x1;
        x2Tot += validBoxes.at(i).x2;
        y1Tot += validBoxes.at(i).y1;
        y2Tot += validBoxes.at(i).y2;
    }
    boundingBox avgBox;
    avgBox.x1 = x1Tot / num;
    avgBox.x2 = x2Tot / num;
    avgBox.y1 = y1Tot / num;
    avgBox.y2 = y2Tot / num;

    imageBBresults imgData;
    imgData.img = img;
    imgData.avgBox = avgBox;
    imgData.foundBoxCount = validBoxes.size();

    /*
    if(imgData.foundBoxCount != 0)
    {
        cout << "USER:Average found box: (" << imgData.avgBox.x1 << "," << imgData.avgBox.y1 << ") -> (" <<  imgData.avgBox.x2 << "," << imgData.avgBox.y2 << ")" << endl;
    } else {
        cout << "USER:No valid boxes found." << endl;
    }
    */

    return imgData;
}

vector<imageBBresults> calcAvgVectorForChain(camChain curCamChain)
{
    vector<imageBBresults> results(curCamChain.size());

    for(unsigned int i = 0; i < curCamChain.size(); i++)
    {
        cout << "Calculating image: " << curCamChain.at(i)->getSourceFilename() << endl;
        results.at(i) = calcAvgBox( curCamChain.at(i) );
    }

    return results;
}

inline unsigned int calcBoxArea(boundingBox box)
{
    return (box.x2 - box.x1) * (box.y2 - box.y1);
}

inline unsigned int calcBoxWidth(boundingBox box)
{
    return box.x2 - box.x1;
}

velDeltaResults evalImageResults(vector<imageBBresults> chainData)
{
    //right now only returns the area-delta, need conversion table to work

    const double timeDeltaBase = 1 / chainData.size();
    //time delta between images is assumed to be the inverse of the number of images captured over 1 second

    #define MINIMUM_VALID_BOXES_NEEDED 3

    velDeltaResults results;
    results.usefulImageCount = 0;
    results.avgWidthDelta = 0.0f;

    int prevImgIdx = -1;

    //try to find the first valid image data
    for(int i = 0; i < chainData.size(); i++)
    {
        if(chainData.at(i).foundBoxCount >= MINIMUM_VALID_BOXES_NEEDED)
        {
            prevImgIdx = i;
            results.usefulImageCount++;
            break;
        }
    }

    //if no valid image found, return 0 useful images data
    if(prevImgIdx == -1)
    {
        return results;
    }

    unsigned int imgIndex = prevImgIdx + 1;
    while(imgIndex < chainData.size())
    {
        //if the current data has the needed valid box count
        if(chainData.at(imgIndex).foundBoxCount >= MINIMUM_VALID_BOXES_NEEDED)
        {
            //increment useful images count
            results.usefulImageCount++;

            //find area and time deltas between the two datasets
            int widthDel = calcBoxWidth(chainData.at(imgIndex).avgBox) - calcBoxWidth(chainData.at(prevImgIdx).avgBox);
            //the time between images is the framerate * the index count between the two image datasets
            double timeDel = timeDeltaBase * (imgIndex - prevImgIdx);

            //divide the area delta between data by the time delta between data
            results.avgWidthDelta += widthDel / timeDel;
            prevImgIdx = imgIndex;
        }

        //advance index to the next image dataset
        imgIndex++;
    }

    results.avgWidthDelta = results.avgWidthDelta / results.usefulImageCount;
    //get the average of all found area-time deltas

    //put code for converting area delta to velocity delta here
    results.avgVelDelta = 0;

    cout << "USER:From " << results.usefulImageCount << " useful images, width delta: " << results.avgWidthDelta << endl;

    return results;
}