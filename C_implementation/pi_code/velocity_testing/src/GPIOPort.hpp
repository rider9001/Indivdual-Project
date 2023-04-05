#ifndef GPIO_CLASS_H
#define GPIO_CLASS_H

#include <string>
#include <fstream>
#include <string>
#include <iostream>
#include <sstream>
#include <stdexcept>

#define GPIO_OUTPUT "out"
#define GPIO_INPUT "in" 

#define GPIO_HIGH "1"
#define GPIO_LOW "0"

using namespace std;

/* GPIO Class
 * Purpose: Each object instantiated from this class will control a GPIO pin
 * The GPIO pin number must be passed to the overloaded class constructor
 */
 
class GPIOPort
{
public:
    GPIOPort(string port); // create a GPIO object that controls GPIOx, where x is passed to this constructor
    GPIOPort(string port, string dir); //more encapsulated GPIO constructor, exports and directions the port

    ~GPIOPort(); //destructor, unexports port
    
    int export_gpio(); // exports GPIO
    int unexport_gpio(); // unexport GPIO
    int setdir_gpio(string dir); // Set GPIO Direction
    int setval_gpio(string val); // Set GPIO Value (putput pins)
    int getval_gpio(); // Get GPIO Value (input/ output pins)

    string get_gpionum(); // return the GPIO number associated with the instance of an object
    string get_direction(); //return direction of port

private:
    string gpionum; // GPIO number associated with the instance of an object
    string direction; //direction of the port
};
#endif
