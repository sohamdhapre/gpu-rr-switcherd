#pragma once
#include <cstdlib>
#include <libudev.h>
#include <string>
#include <fstream>
#include <sstream>



class powerMonitor {
public:
    powerMonitor();
    ~powerMonitor();

    void startListening();
    std::string getBatteryStatus();

private:
    struct udev* udevContext;
    struct udev_monitor* udevMonitor;
    int fd;

};