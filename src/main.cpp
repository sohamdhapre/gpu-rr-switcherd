#include <iostream>
#include <string>
#include "powerMonitor.h"
#include "displayManager.h"
#include "gpuManager.h"

int main (void)
{
    std::cout << "Prog Started\n";

    powerMonitor mon;
    std::string batteryStatus;
    displayManager manager;
    while(1)
    {
    batteryStatus = mon.startListening();

    if(batteryStatus=="Discharging")
    {
        manager.setRefreshRateToMin();
        // gpuManager::setToIntegratedMode();

    }
    else
    {
        manager.setRefreshRateToMax();
        // gpuManager::setToHybridMode();
    }



    }
    return 0;
}
