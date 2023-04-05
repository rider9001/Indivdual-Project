#include <vector>
#include <string>

#include "ImgMtx.hpp"

/*
Basic class to ensure image data is wiped when exiting contexts where it is used
*/

class camChain {
    private:
        std::vector<ImgMtx*> camChainVec;

    public:
        unsigned int size();
        ImgMtx * at(int);

        camChain(std::vector<string>);
        ~camChain();
};