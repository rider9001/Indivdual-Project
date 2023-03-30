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
#include <chrono>
#include <thread>

#define STATS_FILENM "incidentStats.txt"

struct imageBBresults
{
    ImgMtx * img; //note that this pointer is deleted when exiting the context of analysing a directory
    unsigned int foundBoxCount = 0;
    boundingBox avgBox;
    float distance = 0;
};

struct velDeltaResults
{
    float avgWidthDelta;
    float avgVelDelta;
    int usefulImageCount; //basic variable pair to indicate if data rich enough to be useful
};

inline unsigned int calcBoxArea(boundingBox);
inline unsigned int calcBoxWidth(boundingBox);
inline float convertPixWidthToDist(int pixWidth);

void analyseCamChain(string , pauseCtrl&);
imageBBresults calcAvgBox(ImgMtx * img);

velDeltaResults evalImageResults(vector<imageBBresults> &chainData);
velDeltaResults firstLastMethodImageResults(vector<imageBBresults> &chainData);
velDeltaResults directionalExculciveImageResults(vector<imageBBresults> &chainData);