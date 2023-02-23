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

    regex jpgImgRegex("\\S+.jpg$");//regex that matches '*.jpg' filenames

	vector<string> jpgFileNames;
    for (const auto & entry : std::filesystem::directory_iterator(directory))
	{
		if( regex_match(entry.path().c_str(), jpgImgRegex, regex_constants::match_default) )
		{ jpgFileNames.push_back( entry.path().c_str()); }
	}

    std::sort(jpgFileNames.begin(), jpgFileNames.end());//use basic sort to get images in order

    vector<ImgMtx*> captureTrain;
    for(unsigned int i = 0; i < jpgFileNames.size(); i++)
    {captureTrain.push_back( new ImgMtx( jpgFileNames.at(i).c_str() ) );}

    vector<imageBBresults> results = calcAvgVectorForChain(captureTrain);
    auto endT = std::chrono::system_clock::now();

    ofstream statsFile;
    statsFile.open(directory + "/" + STATS_FILENM);

    statsFile << "Incident occoured at: " << directory << endl;
    statsFile << "Total images captured: " << captureTrain.size() << endl;
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

    for(unsigned int i = 0; i < captureTrain.size(); i++)
    {
        delete captureTrain.at(i);
    }
}

imageBBresults calcAvgBox(ImgMtx * img)
{
    //assumes that the image has had no processing done on it whatsoever, aside from greyscaling performed on image read

    img->fullFilter();
    vector<boundingBox> validBoxes = boxFilter(img->getBoundingBoxes(), img->getWidth(), img->getHeight());

    boundingBox avgBox;
    unsigned int x1Tot = 0, x2Tot = 0, y1Tot = 0, y2Tot = 0;
    for(unsigned int i = 0; i < validBoxes.size(); i++)
    {
        x1Tot += validBoxes.at(i).x1;
        x2Tot += validBoxes.at(i).x2;
        y1Tot += validBoxes.at(i).y1;
        y2Tot += validBoxes.at(i).y2;
    }
    avgBox.x1 = x1Tot / validBoxes.size();
    avgBox.x2 = x2Tot / validBoxes.size();
    avgBox.y1 = y1Tot / validBoxes.size();
    avgBox.y2 = y2Tot / validBoxes.size();

    imageBBresults imgData;
    imgData.img = img;
    imgData.avgBox = avgBox;
    imgData.foundBoxCount = validBoxes.size();

    return imgData;
}

vector<imageBBresults> calcAvgVectorForChain(vector<ImgMtx *> camChain)
{
    vector<imageBBresults> results(camChain.size());

    for(unsigned int i = 0; i < camChain.size(); i++)
    {
        //cout << "Calculating image " << i+1 << " in chain." << endl;
        results.at(i) = calcAvgBox( camChain.at(i) );
    }

    return results;
}