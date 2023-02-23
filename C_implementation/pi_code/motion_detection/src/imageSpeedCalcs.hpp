#include "ImgMtx.hpp"
#include "boundingBoxStructs.h"

#include "boxFilter.hpp"

#include <vector>
#include <thread>
#include <future>
#include <string>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <regex>

#define STATS_FILENM "incidentStats.txt"

struct imageBBresults
{
    ImgMtx * img;
    unsigned int foundBoxCount = 0;
    boundingBox avgBox;
};


imageBBresults calcAvgBox(ImgMtx * img);
vector<imageBBresults> calcAvgVectorForChain(vector<ImgMtx *> camChain);
vector<double> getVelDelta(vector<imageBBresults> chainData);

void analyseCamChain(string directory);