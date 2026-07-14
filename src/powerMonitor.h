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
        bool isPluggedIn();

    private:
        struct udev* udevContext;
        struct udev_monitor* udevMonitor;
        int fd;

};