#ifndef FILTERSTAGES_H_INCLUDED
#define FILTERSTAGES_H_INCLUDED

enum filterStage
{
    Grayscale       = 0,
    GaussFiltered   = 1,
    SobelFiltered   = 2,
    nonMaxSupressed = 3,
    edgeLinked      = 4
};

#endif // FILTERSTAGES_H_INCLUDED
