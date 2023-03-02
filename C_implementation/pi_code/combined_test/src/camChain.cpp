#include "camChain.hpp"

camChain::camChain(std::vector<string> fileNms)
{
    for(unsigned int i = 0; i < fileNms.size(); i++)
    {
        camChainVec.push_back( new ImgMtx( fileNms.at(i).c_str() ) );
        cout << "USER:" << camChainVec.at(i)->getSourceFilename() << " loaded sucsessfully" << endl;
    }
}

camChain::~camChain()
{
    for(unsigned int i = 0; i < camChainVec.size(); i++)
    {
        delete camChainVec.at(i);
    }
}

unsigned int camChain::size()
{
    return camChainVec.size();
}

ImgMtx * camChain::at(int i)
{
    return camChainVec.at(i);
}