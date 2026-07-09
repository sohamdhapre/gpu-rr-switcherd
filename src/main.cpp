#include <iostream>
#include "powerMonitor.h"
#include "displayManager.h"
int main (void) 
{
    std::cout << "Prog Started\n";

    // powerMonitor mon;
    // mon.startListening();
    displayManager manager;

    double rr = manager.getCurrentRefreshRate();
    std::cout<<"Current refrest rate is "<<rr<<"Hz\n\n";

    // manager.setRefreshRateToMin();

    // double rr1 = manager.getCurrentRefreshRate();
    // std::cout<<"Current refrest rate is "<<rr1<<"Hz\n\n";

    manager.setRefreshRateToMax();

    double rr2 = manager.getCurrentRefreshRate();
    std::cout<<"Current refrest rate is "<<rr2<<"Hz\n\n";


    return 0;
}