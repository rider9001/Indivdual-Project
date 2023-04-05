#ifndef BOUNDINGBOXSTRUCTS_H_INCLUDED
#define BOUNDINGBOXSTRUCTS_H_INCLUDED

//if you ever need to handle an image larger than 2^16 in any dimension, use another algorithm

struct bbPixel
{
    uint16_t y;
    uint16_t x;
};

struct boundingBox
{
    uint16_t y1=0;
    uint16_t x1=0;
    uint16_t y2=0;
    uint16_t x2=0;
};

#endif // BOUNDINGBOXSTRUCTS_H_INCLUDED
