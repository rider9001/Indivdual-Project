#include <mutex>

class pauseCtrl 
{
    private:
        bool pauseBool;
        std::mutex pauseLock;

    public:
        bool isPaused();
        void pause();
        void unPause();
        pauseCtrl();
};