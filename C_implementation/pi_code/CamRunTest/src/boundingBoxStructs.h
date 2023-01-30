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
    uint16_t y1;
    uint16_t x1;
    uint16_t y2;
    uint16_t x2;
};

#endif // BOUNDINGBOXSTRUCTS_H_INCLUDED
