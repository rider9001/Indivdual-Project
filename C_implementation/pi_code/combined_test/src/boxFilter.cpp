//#define PRINT_INDIVIDUAL_STAGE_PASSES

#include "boxFilter.hpp"

vector<boundingBox> boxFilter(vector<boundingBox> inpBoxes, int sourceImWid, int sourceImHigh)
{
    //takes vector of bounding boxes and filters out suspected licence plate boxes
    //will return blank vector if no any stage has 0 passes

    #ifdef PRINT_INDIVIDUAL_STAGE_PASSES
    cout << inpBoxes.size() << " input boxes" << endl;
    #endif // PRINT_INDIVIDUAL_STAGE_PASSES

    //height to width ratio filter
    vector<boundingBox> ratioPass;
    const double HighWidthRatio = 1.58f; //ratio of height to width of UK licence plates
    const double tolerance = 0.20f; //tolerance of devation from ratio, percentage value
    const double lowRatioBound = HighWidthRatio - (tolerance * HighWidthRatio);
    const double highRatioBound = HighWidthRatio + (tolerance * HighWidthRatio);

    for(unsigned int i = 0; i < inpBoxes.size(); i++)
    {
        //get current bounding box ratio
        boundingBox curBox = inpBoxes.at(i);
        if(curBox.x2 - curBox.x1 == 0)
            continue;//catch case to prevent divide by 0 error, single pixel width boxes would fail anyway

        //calculate ratio, convert uint16 values to double
        double boxRatio = ( (double)curBox.y2 - (double)curBox.y1) / ( (double)curBox.x2 - (double)curBox.x1 );
        //if box is between lowRatioBound and highRatioBound, passes ratio filter
        if( (boxRatio < highRatioBound) && (boxRatio > lowRatioBound) )
        {
            ratioPass.push_back(curBox);
        }
    }

    #ifdef PRINT_INDIVIDUAL_STAGE_PASSES
    cout << ratioPass.size() << " pass ratio filter" << endl;
    #endif //PRINT_INDIVIDUAL_STAGE_PASSES

    //if no boxes pass, return empty vector
    if(ratioPass.size() == 0)
    {
        return ratioPass;
    }

    //width pass range filtering
    vector<boundingBox> widthPass;
    const int lowWidLim = sourceImWid * 0.01; //box width must be above this percent of im width
    const int highWidLim = sourceImWid * 0.15; //box width must be below this percent of im width

    for(unsigned int i = 0; i < ratioPass.size(); i++)
    {
        boundingBox curBox = ratioPass.at(i);
        int curWidth = curBox.x2 - curBox.x1;
        if( (curWidth < highWidLim) && (curWidth > lowWidLim) )
        {
            widthPass.push_back(curBox);
        }
    }

    #ifdef PRINT_INDIVIDUAL_STAGE_PASSES
    cout << widthPass.size() << " pass width filter" << endl;
    #endif

    //if no boxes pass, return empty vector
    if(widthPass.size() == 0)
    {
        return widthPass;
    }

    //round and mode filter y1 coord
    vector<boundingBox> ymodePass;
    vector<int> roundedYCoords;
    const int unitYRounding = 150; //round to closest (this value), decrease to make y coordinate drift exclusion more strict

    //round all boxes y1 coordinates
    for(unsigned int i = 0; i < widthPass.size(); i++)
    {
        uint16_t curY1Coord = widthPass.at(i).y1;

        //find closest multiple of const below the coordiante
        int multiplier = 0;
        while( (multiplier+1) * unitYRounding < curY1Coord )
        {
            multiplier++;
        }

        //check if y coord is closer to upper or lower bound of rounding
        if( ( (multiplier+1)*unitYRounding - curY1Coord ) < ( curY1Coord - multiplier*unitYRounding ) )
        {
            //closer to upper bound
            roundedYCoords.push_back( (multiplier+1)*unitYRounding );
        }
        else
        {
            //closer to lower bound
            roundedYCoords.push_back( multiplier*unitYRounding );
        }
    }

    //find mode of the rounded y coordinates
    int yMode = findMode(roundedYCoords);

    //get boxes with values that match the mode of rounded y coords
    for(unsigned int i = 0; i < roundedYCoords.size(); i++)
    {
        if(roundedYCoords.at(i) == yMode)
        {
            ymodePass.push_back(widthPass.at(i));
        }
    }

    #ifdef PRINT_INDIVIDUAL_STAGE_PASSES
    cout << ymodePass.size() << " pass mode filter" << endl;
    #endif

    //round and filter box area
    vector<boundingBox> areaModePass;
    vector<int> roundedAreas;
    int unitForRoundingArea = 2250;

    //round all boxes areas
    for(unsigned int i = 0; i < ymodePass.size(); i++)
    {
        int curArea = (ymodePass.at(i).y2 - ymodePass.at(i).y1) * (ymodePass.at(i).x2 - ymodePass.at(i).x1);

        //find closest multiple of const below the coordiante
        int multiplier = 0;
        while( (multiplier+1) * unitForRoundingArea < curArea )
        {
            multiplier++;
        }

        //check if y coord is closer to upper or lower bound of rounding
        if( ( (multiplier+1)*unitForRoundingArea - curArea ) < ( curArea - multiplier*unitForRoundingArea ) )
        {
            //closer to upper bound
            roundedAreas.push_back( (multiplier+1)*unitForRoundingArea );
        }
        else
        {
            //closer to lower bound
            roundedAreas.push_back( multiplier*unitForRoundingArea );
        }
    }

    //find mode of the rounded y coordinates
    int areaMode = findMode(roundedAreas);

    //get boxes with values that match the mode of rounded y coords
    for(unsigned int i = 0; i < roundedAreas.size(); i++)
    {
        if(roundedAreas.at(i) == areaMode)
        {
            areaModePass.push_back(ymodePass.at(i));
        }
    }

    #ifdef PRINT_INDIVIDUAL_STAGE_PASSES
    cout << areaModePass.size() << " pass area mode filter" << endl;
    #endif

    //return boxes that pass all stages
    return areaModePass;
}

int findMode(vector<int> values)
{
    //returns most common value from input vector
    vector<int> foundVals, valCount;

    for(unsigned int i = 0; i < values.size(); i++)
    {
        bool valAlreadyRecorded = false;
        for(unsigned int j = 0; j < foundVals.size(); j++)
        {
            if(foundVals.at(j) == values.at(i))
            {
                valAlreadyRecorded = true;
                valCount.at(j)++;
                break;
            }
        }

        if(!valAlreadyRecorded)
        {
            foundVals.push_back(values.at(i));
            valCount.push_back(1);
        }
    }

    int highIdx = 0;
    int highCount = 0;
    for(unsigned int i = 0; i < valCount.size(); i++)
    {
        if(valCount.at(i) > highCount)
        {
            highCount = valCount.at(i);
            highIdx = i;
        }
    }

    return foundVals.at(highIdx);
}
