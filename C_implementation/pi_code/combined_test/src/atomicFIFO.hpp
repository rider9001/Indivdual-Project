#include <queue>
#include <mutex>
#include <string>

/*
Queue encapsulated with a mutex to make a basic atomic queue
*/

class atomicFIFO {
    private:
        std::mutex queueLock;
        std::queue<string> queue;

    public:
        string pop();
        void push(string);
        unsigned int size();
        bool empty();
};