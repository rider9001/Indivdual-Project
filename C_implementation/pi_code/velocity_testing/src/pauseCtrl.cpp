#include "pauseCtrl.hpp"

pauseCtrl::pauseCtrl()
{
    pauseBool = false;
    //start unpaused
}

bool pauseCtrl::isPaused()
{
    std::unique_lock<std::mutex> lck(pauseLock);
    return pauseBool;
}

void pauseCtrl::pause()
{
    std::unique_lock<std::mutex> lck(pauseLock);
    pauseBool = true;
}

void pauseCtrl::unPause()
{
    std::unique_lock<std::mutex> lck(pauseLock);
    pauseBool = false;
}