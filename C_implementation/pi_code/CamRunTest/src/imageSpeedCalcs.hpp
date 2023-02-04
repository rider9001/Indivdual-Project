#include "ImgMtx.hpp"
#include "boundingBoxStructs.h"

#include "boxFilter.hpp"

#include <vector>
#include <thread>
#include <future>

struct imageBBresults
{
    ImgMtx * img;
    unsigned int foundBoxCount = 0;
    boundingBox avgBox;
};


imageBBresults calcAvgBox(ImgMtx * img);
vector<imageBBresults> calcAvgVectorThreaded(vector<ImgMtx *> camChain, unsigned int threadCount);
vector<imageBBresults> calcAvgVectorSingle(vector<ImgMtx *> camChain);
vector<double> getVelDelta(vector<imageBBresults> chainData);