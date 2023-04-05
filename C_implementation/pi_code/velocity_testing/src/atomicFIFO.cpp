#include "atomicFIFO.hpp"

void atomicFIFO::push(string inp)
{
    std::unique_lock<std::mutex> lck(queueLock);
    queue.push(inp);
}

string atomicFIFO::pop()
{
    std::unique_lock<std::mutex> lck(queueLock);
    string out = queue.front();
    queue.pop();
    return out;
}

unsigned int atomicFIFO::size()
{
    std::unique_lock<std::mutex> lck(queueLock);
    return queue.size();
}

bool atomicFIFO::empty()
{
    return queue.empty();
}