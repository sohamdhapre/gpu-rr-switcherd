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
        

        if(!mon.isPluggedIn())
        {
            manager.setRefreshRateToMin();
            
            if(gpuManager::getGPUMode() !=  Mode::integrated)
            {
                gpuManager::promptRestart(Mode::integrated);
            }   
            
        }
        else
        {
            manager.setRefreshRateToMax();
            if(gpuManager::getGPUMode() != Mode::hybrid)
            {
                gpuManager::promptRestart(Mode::hybrid);
            }
        }
        mon.startListening();

    }

    return 0;
}
