#include "imageSpeedCalcs.hpp"

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

vector<imageBBresults> calcAvgVectorThreaded(vector<ImgMtx *> camChain, unsigned int threadCount)
{
    vector<imageBBresults> results(camChain.size());
    vector<std::thread> imageProcessors(threadCount);
    unsigned int nextImgIdx = 0;

    while(nextImgIdx < camChain.size())
    {
        for(unsigned int i = 0; i < imageProcessors.size(); i++)
        {
            if(nextImgIdx >= camChain.size()) {break;}
            unsigned int imgIdx = nextImgIdx++;

            cout << "Creating thread " << i << " for image " << imgIdx << endl;

            imageProcessors.at(i) = std::thread([&](){
                unsigned int returnIdx = imgIdx;
                results.at(returnIdx) = calcAvgBox( camChain.at(returnIdx) );
            });
        }

        for(unsigned int i = 0; i < imageProcessors.size(); i++)
        {
            if(imageProcessors.at(i).joinable())
            {
                cout << "joining thread " << i << endl;
                imageProcessors.at(i).join();
            }
        }
    }

    return results;
}

vector<imageBBresults> calcAvgVectorSingle(vector<ImgMtx *> camChain)
{
    vector<imageBBresults> results(camChain.size());

    for(unsigned int i = 0; i < camChain.size(); i++)
    {
        results.at(i) = calcAvgBox( camChain.at(i) );
    }

    return results;
}