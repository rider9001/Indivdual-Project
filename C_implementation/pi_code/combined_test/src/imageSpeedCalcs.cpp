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

    //WHYYYYYYYY, fix this
    vector<imageBBresults> results(curCamChain.size());
    for(unsigned int i = 0; i < curCamChain.size(); i++)
    {
        cout << "Calculating image: " << curCamChain.at(i)->getSourceFilename() << endl;
        results.at(i) = calcAvgBox( curCamChain.at(i) );
    }

    auto endT = std::chrono::system_clock::now();

    ofstream statsFile;
    statsFile.open(directory + "/" + STATS_FILENM);

    statsFile << "Incident occoured at: " << directory << endl;
    statsFile << "Total images captured: " << curCamChain.size() << endl;
    statsFile << endl;

    for(unsigned int i = 0; i < results.size(); i++)
    {
        imageBBresults curRes = results.at(i);

        statsFile << "------ Image " << (i+1) << " stats ------" << endl;
        statsFile << "Filename: " << curRes.img->getSourceFilename() << endl;
        statsFile << "Plate matches found: " << curRes.foundBoxCount << endl;
        statsFile << "Average box: (" << curRes.avgBox.x1 << "," << curRes.avgBox.y1 << ") -> (" << curRes.avgBox.x2 << "," << curRes.avgBox.y2 << ")" << endl;
        statsFile << "Average box size: " << (curRes.avgBox.x2 - curRes.avgBox.x1) * (curRes.avgBox.y2 - curRes.avgBox.y1) << endl;

        statsFile << endl;
    }

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
    auto startT = std::chrono::system_clock::now();
    img->fullFilter();
    auto endT = std::chrono::system_clock::now();

    std::chrono::duration<double> totalTime = endT - startT;
    //cout << "USER:Filtering completed in: " << totalTime.count() << "s" << endl;

    startT = std::chrono::system_clock::now();
    vector<boundingBox> validBoxes = boxFilter(img->getBoundingBoxes(), img->getWidth(), img->getHeight());
    endT = std::chrono::system_clock::now();

    totalTime = endT - startT;
    //cout << "USER:Box finding completed in: " << totalTime.count() << "s" << endl;

    unsigned int x1Tot = 0, x2Tot = 0, y1Tot = 0, y2Tot = 0;
    for(unsigned int i = 0; i < validBoxes.size(); i++)
    {
        x1Tot += validBoxes.at(i).x1;
        x2Tot += validBoxes.at(i).x2;
        y1Tot += validBoxes.at(i).y1;
        y2Tot += validBoxes.at(i).y2;
    }
    boundingBox avgBox;
    avgBox.x1 = x1Tot / validBoxes.size();
    avgBox.x2 = x2Tot / validBoxes.size();
    avgBox.y1 = y1Tot / validBoxes.size();
    avgBox.y2 = y2Tot / validBoxes.size();

    imageBBresults imgData;
    imgData.img = img;
    imgData.avgBox = avgBox;
    imgData.foundBoxCount = validBoxes.size();

    cout << "Average found box: (" << imgData.avgBox.x1 << "," << imgData.avgBox.y1 << ") -> (" <<  imgData.avgBox.x2 << "," << imgData.avgBox.y2 << ")" << endl;

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