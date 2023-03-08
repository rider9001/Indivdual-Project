#include "GPIOPort.cpp"

#include <chrono>
#include <thread>

using namespace std;

int main()
{
    GPIOPort port14("14", GPIO_OUTPUT);
    GPIOPort port15("15", GPIO_INPUT);

    for(;;)
    {
        port14.setval_gpio(GPIO_HIGH);
        std::this_thread::sleep_until(std::chrono::system_clock::now() + std::chrono::milliseconds(500));
        port14.setval_gpio(GPIO_LOW);
        std::this_thread::sleep_until(std::chrono::system_clock::now() + std::chrono::milliseconds(500));

        cout << "Value of port " << port15.get_gpionum() << ": " << port15.getval_gpio() << endl;
    }
}