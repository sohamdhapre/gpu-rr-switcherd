#include "powerMonitor.h"
#include <iostream>
#include <poll.h>
#include <unistd.h>
#include <fstream>

powerMonitor::powerMonitor()
{
    udevContext = udev_new();
    if (!udevContext) 
    {
        std::cerr << "Failed to create udev context" << std::endl;
        exit(1);
    }

    udevMonitor = udev_monitor_new_from_netlink(udevContext, "udev");
    if (!udevMonitor) 
    {
        std::cerr << "Failed to create udev monitor." << std::endl;
        udev_unref(udevContext);
        exit(1);
    }

    udev_monitor_filter_add_match_subsystem_devtype(udevMonitor, "power_supply", NULL);

    if(udev_monitor_enable_receiving(udevMonitor) < 0)
    {
        std::cerr << "Failed to bind the udev monitor." << std::endl;
        udev_monitor_unref(udevMonitor);
        udev_unref(udevContext);
        exit(1);
    }

    fd = udev_monitor_get_fd(udevMonitor);

}


    powerMonitor::~powerMonitor()
{
    if (udevMonitor) udev_monitor_unref(udevMonitor);
    if (udevContext) udev_unref(udevContext);
}


std::string powerMonitor::startListening()
{
    std::cout <<"Start listening called\n";
    struct pollfd pfd;
    pfd.fd = fd;
    pfd.events = POLLIN;

    
    
    int ret = poll(&pfd, 1, -1);

    if(ret > 0 && (pfd.revents && POLLIN))
    {
        struct udev_device* device = udev_monitor_receive_device(udevMonitor);
        std::string devnode = udev_device_get_sysname(device);
        if (devnode.find("AC") != std::string::npos || devnode.find("ADP") != std::string::npos) 
        {
            std::string action = udev_device_get_action(device);
            std::cout << "Hardware Event Triggered: action:" << action << " on devnode:" << devnode << "\n";
            std::cout << "Battery status is: " << powerMonitor::getBatteryStatus() << '\n';

            return getBatteryStatus();
            
        }
    }
    return getBatteryStatus();

}

std::string powerMonitor::getBatteryStatus()
{
    std::ifstream file ("/sys/class/power_supply/BAT0/status");

    if (!file.is_open()) 
    {
        std::cerr << "Error opening file!\n";
        exit(1);
    }

    std::string status;
    getline(file, status);
    return status;
}