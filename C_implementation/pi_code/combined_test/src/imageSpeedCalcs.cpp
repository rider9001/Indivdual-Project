#include <chrono>
#include <ctime>
#include <vector>
#include <string>
#include <regex>

#include "imageSpeedCalcs.hpp"

 #define MINIMUM_VALID_BOXES_NEEDED 3

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
        cout << "USER: " << i+1 << " / " << curCamChain.size() << "processed \r";
    }

    /*
    //uncomment to dump image filter results
    for(unsigned int i = 0; i < curCamChain.size(); i++)
    {
        string outName = to_string(i+1) + "_out.jpg";
        curCamChain.at(i)->writeImg( outName.c_str() );
    }
    */

    velDeltaResults velResults = evalImageResults(imgResults);

    auto endT = std::chrono::system_clock::now();
    std::chrono::duration<double> totalTime = endT - startT;

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

        if(curRes.foundBoxCount >= MINIMUM_VALID_BOXES_NEEDED)
        {
            statsFile << "Average box: (" << curRes.avgBox.x1 << "," << curRes.avgBox.y1 << ") -> (" << curRes.avgBox.x2 << "," << curRes.avgBox.y2 << ")" << endl;
            statsFile << "Average box width: " << calcBoxWidth(curRes.avgBox) << endl;
            statsFile << "Plate distance: " << curRes.distance << "mm" << endl;
        } else {
            statsFile << "Too few matching boxes to be used in calcations." << endl;
        }

        statsFile << endl;
    }

    statsFile << "Useful images in image chain: " << velResults.usefulImageCount << endl;
    if(velResults.usefulImageCount > 2)
    {
        statsFile << "Average width delta: " << velResults.avgWidthDelta << "pix/s" << endl;
        statsFile << "Average velocity delta: " << velResults.avgVelDelta << "mm/s" << endl;
    } else {
        statsFile << "Not enough useful images to make velocity calculation" << endl;
    }

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

inline unsigned int calcBoxArea(boundingBox box)
{
    return (box.x2 - box.x1) * (box.y2 - box.y1);
}

inline unsigned int calcBoxWidth(boundingBox box)
{
    return box.x2 - box.x1;
}

velDeltaResults evalImageResults(vector<imageBBresults> &chainData)
{
    //right now only returns the area-delta, need conversion table to work

    const float timeDeltaBase = 0.0166f;
    //time delta between images has been calculated as 16.6ms

    //orignal = 1050850.0f, recalcuated = 1210850.0f
    const float k_val = 1210850.0f;
    const float realWorldWidth = 0.043f; //50mm in meters, or the width of whatever is being measured

    /*
    Distance and pixel width of an object are inversely related P = k/d
    rearraging to d = k/P the distance from the camera can be found
    k value calcated from data in source images folder
    NOTE: P is pixels per m, divided measured pixel length by real world length of the object
    */ 

    vector<int> pixWidths(chainData.size());
    vector<float> distances(chainData.size()); //all calcated distances are in mm

    for(int i = 0; i < chainData.size(); i++)
    {
        pixWidths.at(i) = calcBoxWidth(chainData.at(i).avgBox);
        float pixPerM = pixWidths.at(i) / realWorldWidth;
        distances.at(i) = k_val / pixPerM;
        chainData.at(i).distance = distances.at(i);

        if(chainData.at(i).foundBoxCount >= MINIMUM_VALID_BOXES_NEEDED)
        {
            cout << "USER:" << i+1 << " width: " << pixWidths.at(i) << ", dist: " << distances.at(i) << "mm" << endl;
        }
    }

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

            //the time between images is the framerate * the index count between the two image datasets
            float timeDel = timeDeltaBase * (imgIndex - prevImgIdx);
            cout << "USER:Time diff: " << timeDel << "s" << endl;  

            //find area and time deltas between the two datasets
            int widthDel = pixWidths.at(imgIndex) - pixWidths.at(prevImgIdx);
            //divide the area delta between data by the time delta between data
            results.avgWidthDelta += widthDel / timeDel;

            //find same for distance delta (velocity)
            float distDel = distances.at(imgIndex) - distances.at(prevImgIdx);

            cout << "USER:Vel for " << prevImgIdx << " to " << imgIndex << endl;
            cout << "USER: " << distances.at(prevImgIdx)  << "mm to " << distances.at(imgIndex) << "mm" << endl;
            cout << "USER:Distance delta " << distDel << "mm" << endl;
            cout << "USER:Velocity: " << distDel / (timeDel * 1000) << "m/s" <<  endl;
            cout << endl;
            results.avgVelDelta += distDel / timeDel;

            prevImgIdx = imgIndex;
        }

        //advance index to the next image dataset
        imgIndex++;
    }

    results.avgWidthDelta = results.avgWidthDelta / results.usefulImageCount;
    //get the average of all found area-time deltas

    //get average for all found velocities
    results.avgVelDelta = results.avgVelDelta / results.usefulImageCount;

    cout << "USER:From " << results.usefulImageCount << " useful images, width delta: " << results.avgWidthDelta << "pix/s" << endl;
    cout << "USER:Vel delta: " << results.avgVelDelta << "mm/s" << endl;

    return results;
}