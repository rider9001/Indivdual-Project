#include "ImgMtx.hpp"
#include "boundingBoxStructs.h"
#include "boxFilter.hpp"
#include "camChain.cpp"

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
    ImgMtx * img; //note that this pointer is deleted when exiting the context of analysing a directory
    unsigned int foundBoxCount = 0;
    boundingBox avgBox;
};

struct velDeltaResults
{
    double avgWidthDelta;
    double avgVelDelta;
    int usefulImageCount; //basic variable pair to indicate if data rich enough to be useful
};

inline unsigned int calcBoxArea(boundingBox);
inline unsigned int calcBoxWidth(boundingBox);

void analyseCamChain(string directory);
imageBBresults calcAvgBox(ImgMtx * img);
vector<imageBBresults> calcAvgVectorForChain(camChain);

velDeltaResults evalImageResults(vector<imageBBresults> chainData);