#define PRINT_INDIVIDUAL_STAGE_PASSES 1

vector<boundingBox> boxFilter(vector<boundingBox> inpBoxes, int sourceImWid, int sourceImHigh)
{
    //takes vector of bounding boxes and filters out suspected licence plate boxes
    //will return blank vector if no any stage has 0 passes

    #ifdef PRINT_INDIVIDUAL_STAGE_PASSES
    cout << inpBoxes.size() << " input boxes" << endl;
    #endif // PRINT_INDIVIDUAL_STAGE_PASSES

    //height to width ratio filter
    vector<boundingBox> ratioPass;
    const double HighWidthRatio = 1.58; //ratio of height to width of UK licence plates
    const double tolerance = 0.15; //tolerance of devation from ration, percentage value
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
    if(ratioPass.size() == 0)
    {
        return ratioPass;
    }

    //round and mode filter area and y coord

    return ratioPass;
}
