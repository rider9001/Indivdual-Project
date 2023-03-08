#include "GPIOPort.hpp"

using namespace std;

/*
NOTE:   There is absolutley no verification on if port numbers or reads/writes are valid, use at own risk
*/

GPIOPort::GPIOPort(string gnum)
{
    this->gpionum = gnum;  //Instatiate GPIOPort object for GPIO pin number "gnum"
}

GPIOPort::GPIOPort(string port, string dir)
{
    this->gpionum = port;

    if(dir != GPIO_INPUT and dir != GPIO_OUTPUT)
    {
        throw std::invalid_argument(dir + " is not a port direction, use strings 'in' or 'out'");
    }
    this->direction = dir;

    this->export_gpio();
    this->setdir_gpio(this->get_direction());
}

GPIOPort::~GPIOPort()
{
    this->unexport_gpio();
}

int GPIOPort::export_gpio()
{
    string export_str = "/sys/class/gpio/export";
    ofstream exportgpio(export_str.c_str()); // Open "export" file. Convert C++ string to C string. Required for all Linux pathnames

    exportgpio << this->gpionum ; //write GPIO number to export
    exportgpio.close(); //close export file
    return 0;
}

int GPIOPort::unexport_gpio()
{
    string unexport_str = "/sys/class/gpio/unexport";
    ofstream unexportgpio(unexport_str.c_str()); //Open unexport file

    unexportgpio << this->gpionum ; //write GPIO number to unexport
    unexportgpio.close(); //close unexport file
    return 0;
}

int GPIOPort::setdir_gpio(string dir)
{
    if(dir != GPIO_INPUT and dir != GPIO_OUTPUT)
    {
        throw std::invalid_argument(dir + " is not a port direction, use strings 'in' or 'out', port: " + this->gpionum);
    }

    string setdir_str ="/sys/class/gpio/gpio" + this->gpionum + "/direction";
    ofstream setdirgpio(setdir_str.c_str()); // open direction file for gpio

    setdirgpio << dir; //write direction to direction file
    setdirgpio.close(); // close direction file

    direction = dir;

    return 0;
}

int GPIOPort::setval_gpio(string val)
{
    if(this->get_direction() == GPIO_INPUT)
    {
        throw std::invalid_argument(this->gpionum + " is an input port, and cannot be written to.");
    }

    if(val != GPIO_HIGH and val != GPIO_LOW)
    {
        throw std::invalid_argument(val + " is not a valid value, use symbols 'GPIO_HIGH' or 'GPIO_LOW'.");
    }

    string setval_str = "/sys/class/gpio/gpio" + this->gpionum + "/value";
    ofstream setvalgpio(setval_str.c_str()); // open value file for gpio

    setvalgpio << val ;//write value to value file
    setvalgpio.close();// close value file
    return 0;
}

int GPIOPort::getval_gpio()
{
    if(this->get_direction() == GPIO_OUTPUT)
    {
        throw std::invalid_argument(this->gpionum + " is an output port, and cannot be read from.");
    }

    string getval_str = "/sys/class/gpio/gpio" + this->gpionum + "/value";
    string val;
    ifstream getvalgpio(getval_str.c_str());// open value file for gpio

    getvalgpio >> val ;  //read gpio value
    getvalgpio.close(); //close the value file
    

    if(val == "1")
    {
        return 1;
    } else {
        return 0;
    }
}
string GPIOPort::get_gpionum()
{
    return this->gpionum;
}

string GPIOPort::get_direction()
{
    return this->direction;
}
