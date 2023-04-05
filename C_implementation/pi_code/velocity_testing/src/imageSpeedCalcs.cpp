#include <chrono>
#include <ctime>
#include <vector>
#include <string>
#include <regex>

#include "imageSpeedCalcs.hpp"

#define MINIMUM_VALID_BOXES_NEEDED 3
#define MINIMUM_VALID_IMAGES_FOR_VELOCITY 6

void analyseCamChain(string directory, pauseCtrl& threadPause)
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

    //calculating the image boxes is the only intenssize part of the algorithm, thus
    //the pause only affects this section

    for(unsigned int i = 0; i < curCamChain.size(); i++)
    {
        while(threadPause.isPaused())
        {
            std::this_thread::sleep_until( std::chrono::system_clock::now() + std::chrono::milliseconds(250) );
        }
        imgResults.at(i) = calcAvgBox( curCamChain.at(i) );
    }

    /*
    //uncomment to dump image filter results
    for(unsigned int i = 0; i < curCamChain.size(); i++)
    {
        string outName = to_string(i+1) + "_out.jpg";
        curCamChain.at(i)->writeImg( outName.c_str() );
    }
    */

    velDeltaResults velResults = highestLowest(imgResults);

    auto endT = std::chrono::system_clock::now();
    std::chrono::duration<double> totalTime = endT - startT;

    ofstream statsFile;
    statsFile.open(directory + "/" + STATS_FILENM);

    statsFile << "Incident occoured at: " << directory << endl;
    statsFile << "Total images captured: " << curCamChain.size() << endl;
    statsFile << endl;

    statsFile << "Useful images in image chain: " << velResults.usefulImageCount << endl;
    statsFile << "Average width delta: " << velResults.avgWidthDelta << "pix/s" << endl;
    statsFile << "Average velocity delta: " << velResults.avgVelDelta << "mm/s" << endl;

    if(velResults.usefulImageCount < MINIMUM_VALID_IMAGES_FOR_VELOCITY)
    {
        statsFile <<  "WARNING!!! Useful image count low, velocity calcuation unlikely to be accuracte." << endl;
    }

    statsFile << "Time to complete analysis: " << totalTime.count() << "s" << endl;

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

inline float convertPixWidthToDist(int pixWidth)
{
    //orignal = 1050850.0f, recalcuated = 1210850.0f
    const float k_val = 1400850.0f;
    const float realWorldWidth = 0.0185f; //50mm in meters, or the width of whatever is being measured

    /*
    Distance and pixel width of an object are inversely related P = k/d
    rearraging to d = k/P the distance from the camera can be found
    k value calcated from data in source images folder
    NOTE: P is pixels per m, divided measured pixel length by real world length of the object
    */ 

   //k_val / pixels per mm
   return k_val / (pixWidth / realWorldWidth); //return is in mm dist from camera
}

velDeltaResults evalImageResults(vector<imageBBresults> &chainData)
{
    const float timeDeltaBase = 0.0166f;
    //time delta between images has been calculated as 16.6ms

    vector<int> pixWidths(chainData.size());
    vector<float> distances(chainData.size()); //all calcated distances are in mm

    for(int i = 0; i < chainData.size(); i++)
    {
        pixWidths.at(i) = calcBoxWidth(chainData.at(i).avgBox);
        distances.at(i) = convertPixWidthToDist(pixWidths.at(i));
        chainData.at(i).distance = distances.at(i);
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
            //cout << "USER:Time diff: " << timeDel << "s" << endl;  

            //find area and time deltas between the two datasets
            int widthDel = pixWidths.at(imgIndex) - pixWidths.at(prevImgIdx);
            //divide the area delta between data by the time delta between data
            results.avgWidthDelta += widthDel / timeDel;

            //find same for distance delta (velocity)
            float distDel = distances.at(imgIndex) - distances.at(prevImgIdx);

            /*
            cout << "USER:Vel for " << prevImgIdx << " to " << imgIndex << endl;
            cout << "USER: " << distances.at(prevImgIdx)  << "mm to " << distances.at(imgIndex) << "mm" << endl;
            cout << "USER:Distance delta " << distDel << "mm" << endl;
            cout << "USER:Velocity: " << distDel / (timeDel * 1000) << "m/s" <<  endl;
            cout << endl;
            */

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

    cout << "USER:Standard method used" << endl;
    cout << "USER:From " << results.usefulImageCount << " useful images, width delta: " << results.avgWidthDelta << "pix/s" << endl;
    cout << "USER:Vel delta: " << results.avgVelDelta << "mm/s" << endl;

    return results;
}

velDeltaResults directionalExculciveImageResults(vector<imageBBresults> &chainData)
{
    const float timeDeltaBase = 0.0166f;
    //time delta between images has been calculated as 16.6ms

    vector<float> pixWidths(chainData.size());
    vector<float> distances(chainData.size()); //all calcated distances are in mm

    for(int i = 0; i < chainData.size(); i++)
    {
        pixWidths.at(i) = calcBoxWidth(chainData.at(i).avgBox);
        distances.at(i) = convertPixWidthToDist(pixWidths.at(i));
        chainData.at(i).distance = distances.at(i);
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

    //create vectors to contain the positive and negative deltas
    vector<float> posWidDel, negWidDel;
    vector<float> posVelDel, negVelDel;

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
            //cout << "USER:Time diff: " << timeDel << "s" << endl;  

            //find area and time deltas between the two datasets
            float widthDel = (pixWidths.at(imgIndex) - pixWidths.at(prevImgIdx)) / timeDel;
            //add delta to the list with the correct direction, zeros are added to both lists
            if(widthDel < 0)
            {
                negWidDel.push_back(widthDel);
            } else if (widthDel > 0) {
                posWidDel.push_back(widthDel);
            } else if(widthDel == 0) {
                posWidDel.push_back(widthDel);
                negWidDel.push_back(widthDel);
            }

            //find same for distance delta (velocity)
            float distDel = (distances.at(imgIndex) - distances.at(prevImgIdx)) / timeDel;

            if(distDel < 0)
            {
                negVelDel.push_back(distDel);
            } else if (distDel > 0) {
                posVelDel.push_back(distDel);
            } else if(distDel == 0) {
                posVelDel.push_back(distDel);
                negVelDel.push_back(distDel);
            }

            prevImgIdx = imgIndex;
        }

        //advance index to the next image dataset
        imgIndex++;
    }

    //select width direction with the higher number of deltas in favour
    //keep in mind increasing width means decreasing distance from camera
    //so if positive width is selected negtative vel should be selected
    //in a tie the negative vel direction will be selected, but this should functionally never happen with a valid camera run
    if(posWidDel.size() >= negWidDel.size())
    {
        cout << "USER:pos wid picked" << endl;
        float sum = 0;
        for(unsigned int i = 0; i < posWidDel.size(); i++)
        { sum += posWidDel.at(i);
        cout << posWidDel.at(i) << endl; }

        results.avgWidthDelta = sum / posWidDel.size();
    } else {
        cout << "USER:neg wid picked" << endl;
        float sum = 0;
        for(unsigned int i = 0; i < negWidDel.size(); i++)
        { sum += negWidDel.at(i);
        cout << negWidDel.at(i) << endl; }

        results.avgWidthDelta = sum / negWidDel.size();
    }

    //this should always return a number opposite in polarity to what the width delta returns
    if(posVelDel.size() >= negVelDel.size())
    {
        cout << "USER:pos vel picked" << endl;
        float sum = 0;
        for(unsigned int i = 0; i < posVelDel.size(); i++)
        { sum += posVelDel.at(i);
        cout << posVelDel.at(i) << endl; }

        results.avgVelDelta = sum / posVelDel.size();
    } else {
        cout << "USER:neg vel picked" << endl;
        float sum = 0;
        for(unsigned int i = 0; i < negVelDel.size(); i++)
        { sum += negVelDel.at(i);
        cout << negVelDel.at(i) << endl; }

        results.avgVelDelta = sum / negVelDel.size();
    }

    cout << "USER:Directional exculsivity method" << endl;
    cout << "USER:From " << results.usefulImageCount << " useful images, width delta: " << results.avgWidthDelta << "pix/s" << endl;
    cout << "USER:Vel delta: " << results.avgVelDelta << "mm/s" << endl;

    return results;
}

velDeltaResults firstLastMethodImageResults(vector<imageBBresults> &chainData)
{
    //test method, may work but should take with grain of salt

    const float timeDeltaBase = 0.01633f;
    //time delta between images has been calculated as 16.33ms


    vector<int> pixWidths(chainData.size());
    for(int i = 0; i < chainData.size(); i++)
    {
        pixWidths.at(i) = calcBoxWidth(chainData.at(i).avgBox);
        chainData.at(i).distance = convertPixWidthToDist(pixWidths.at(i));
    }

    //find first and last valid image index
    int firstUsefulIdx = -1, lastUsefulIdx = -1;
    unsigned int usefulImages = 0;
    for(unsigned int i = 0; i < chainData.size(); i++)
    {
        if(chainData.at(i).foundBoxCount >= MINIMUM_VALID_BOXES_NEEDED)
        {
            usefulImages++;
            if(firstUsefulIdx == -1)
            {
                //first valid image found
                firstUsefulIdx = i;
            } else {
                lastUsefulIdx = i;
                //will be overwritten as new valid boxes are found
            }
        }
    }

    cout << "First image idx: " << firstUsefulIdx << ", last useful idx: " << lastUsefulIdx << endl;

    //get the time between the two images
    float timeDel = timeDeltaBase * (lastUsefulIdx - firstUsefulIdx);

    velDeltaResults results;
    if(usefulImages == 0)
    {
        results.avgVelDelta = 0;
        results.avgWidthDelta = 0;
        results.usefulImageCount = 0;
    } else {
        results.avgWidthDelta = (pixWidths.at(lastUsefulIdx) - pixWidths.at(firstUsefulIdx)) / timeDel;
        results.avgVelDelta = ( chainData.at(lastUsefulIdx).distance - chainData.at(firstUsefulIdx).distance ) / timeDel;
        results.usefulImageCount = usefulImages;
    }

    cout << "USER:First-last average used" << endl;
    cout << "USER:From " << results.usefulImageCount << " useful images, width delta: " << results.avgWidthDelta << "pix/s" << endl;
    cout << "USER:Vel delta: " << results.avgVelDelta << "mm/s" << endl;
    cout << "USER:Time delta: " << timeDel << endl;

    return results;
}

velDeltaResults highestLowest(vector<imageBBresults> &chainData)
{
    //test method, find the highest and lowest distance from useful image set and assume this to be the start and end distantce for entire chain

    const float timeDeltaBase = 0.01633f;
    //time delta between images has been calculated as 16.33ms

    vector<int> pixWidths(chainData.size());
    for(int i = 0; i < chainData.size(); i++)
    {
        pixWidths.at(i) = calcBoxWidth(chainData.at(i).avgBox);
        chainData.at(i).distance = convertPixWidthToDist(pixWidths.at(i));
    }

    int highestWid = -1, lowestWid = 99999;
    int highIdx, lowIdx;
    int usefulImageCount = 0;

    for(unsigned int i = 0; i < pixWidths.size(); i++)
    {
        if(chainData.at(i).foundBoxCount >= MINIMUM_VALID_BOXES_NEEDED)
        {
            usefulImageCount++;
            if(pixWidths.at(i) > highestWid)
            {
                highestWid = pixWidths.at(i);
                highIdx = i;
            }

            if(pixWidths.at(i) < lowestWid)
            {
                lowestWid = pixWidths.at(i);
                lowIdx = i;
            }
        }
    }

    float timeDel = timeDeltaBase * 20;

    velDeltaResults results;
    results.usefulImageCount = usefulImageCount;
    results.avgWidthDelta = (lowestWid - highestWid) / (timeDeltaBase * 20);
    results.avgVelDelta = convertPixWidthToDist(lowestWid - highestWid) / timeDel;

    cout << "USER:highest-lowest used" << endl;
    cout << "USER:From " << results.usefulImageCount << " useful images, width delta: " << results.avgWidthDelta << "pix/s" << endl;
    cout << "USER:Vel delta: " << results.avgVelDelta << "mm/s" << endl;
    cout << "USER:Time delta: " << timeDel << endl;

    return results;
}