#include "displayManager.h"
#include <iostream>
#include<vector>
#include<map>
#include <sdbus-c++/sdbus-c++.h>
#include <unistd.h>
#include <sys/types.h>


//Connector Info: (connector_name, vendor, product, serial)
using ConnectorInfo = sdbus::Struct<std::string, std::string, std::string, std::string>;

// Mode Info: (mode_id, width, height, refresh_rate, preferred_scale, supported_scales, properties)
using ModeInfo = sdbus::Struct<std::string, int32_t, int32_t, double, double, std::vector<double>, std::map<std::string, sdbus::Variant>>;

//Full Monitor Object: (ConnectorInfo, array_of_modes, monitor_properties)
using MonitorStruct = sdbus::Struct<ConnectorInfo, std::vector<ModeInfo>, std::map<std::string, sdbus::Variant>>;

//Logical Monitor Object (X, Y, Scaling, Transform, isPrimary, ConnectorInfo, Properties)
using LogicalMonitorStruct = sdbus::Struct<int32_t, int32_t, double, uint32_t, bool, std::vector<ConnectorInfo>, std::map<std::string, sdbus::Variant>>;

// (connector_name, target_mode_id, properties)
using ApplyConnectorInfo = sdbus::Struct<std::string, std::string, std::map<std::string, sdbus::Variant>>;

// (x, y, scale, transform,isPrimary, list_of_connectors)
using ApplyLogicalMonitor = sdbus::Struct<int32_t, int32_t, double, uint32_t, bool, std::vector<ApplyConnectorInfo>>;

// using displayState = struct 
// {
//     uint32_t serial = 0;
//     std::vector<MonitorStruct> monitors;
//     std::vector<LogicalMonitorStruct> logicalMonitors;
//     std::map<std::string, sdbus::Variant> properties;
// };


// std::unique_ptr<sdbus::IConnection> displayManager::connection = nullptr;
// std::unique_ptr<sdbus::IProxy> displayManager::displayConfigProxy = nullptr;

displayManager::displayManager()
{
    initDBus();
}

const char* sudoUidStr = std::getenv("SUDO_UID");
uid_t user_uid = sudoUidStr ? std::stoi(sudoUidStr) : user_uid;
uid_t root_uid = geteuid();

int displayManager::initDBus()
{

    
    try 
    {   
        
        

        std::string runtimeDir = "/run/user/" + std::to_string(user_uid);
        std::string busAddress = "unix:path=" + runtimeDir + "/bus";
        
        setenv("XDG_RUNTIME_DIR", runtimeDir.c_str(), 1);
        setenv("DBUS_SESSION_BUS_ADDRESS", busAddress.c_str(), 1);

        if (seteuid(user_uid) != 0) 
        {
            std::cerr << "Error: Failed to drop privileges to UID " << user_uid << "\n";
        }
        else{
        std::cout<<"Dropped to UID: "<<user_uid<<std::endl;
        }
        connection = sdbus::createSessionBusConnection();
        


        displayConfigProxy = sdbus::createProxy
        (
            *connection, 
            sdbus::ServiceName("org.gnome.Mutter.DisplayConfig"), 
            sdbus::ObjectPath("/org/gnome/Mutter/DisplayConfig")
        );
        std::cout<<"DBus initialised\n";
        if (seteuid(root_uid) != 0) 
        {
            std::cerr << "Error: Failed to restore root privileges!\n";
            exit(1);
        }
        return 1;
    } 
    catch (const sdbus::Error& e) 
    {
        std::cerr << "Error: D-Bus connection failed: " << e.getMessage() << std::endl;
        exit (1);
    }
}



double displayManager::getCurrentRefreshRate()
{
    std::cout<<"getCurrentRefreshRate called\n";
   
    displayState currentState = displayManager::getCurrentState();

    for (const auto& monitor : currentState.monitors)
    {

        ConnectorInfo connInfo = monitor.template get<0>();

        std::vector<ModeInfo> modes = monitor.template get<1>();

        for(const auto& mode: modes)
        {
            auto modeProperties = mode.template get<6>();
            bool isCurrent = 0;
            
            if(modeProperties.count("is-current"))
            {
                isCurrent = modeProperties.at("is-current").get<bool>();
            }
            if(isCurrent)
            {
                std::cout<<"Mode ID: "<<mode.get<0>()<<std::endl;
                return mode.get<3>();
            }

        }


    }

    return -1;

}

displayState displayManager::getCurrentState()
{
    uint32_t serial = 0;
    std::vector<MonitorStruct> monitors;
    std::vector<LogicalMonitorStruct> logicalMonitors;
    std::map<std::string, sdbus::Variant> properties;
    // if (seteuid(user_uid) != 0) 
    // {
    //     std::cerr << "Error: Failed to drop privileges to UID " << user_uid << "\n";
    // }
    // else{
    //     std::cout<<"Dropped to UID: "<<user_uid<<std::endl;
    // }
    std::cout<<"Calling GetCurrentState Method"<<std::endl;
        displayConfigProxy->callMethod("GetCurrentState")
                        .onInterface("org.gnome.Mutter.DisplayConfig")  
                        .storeResultsTo(serial, monitors, logicalMonitors, properties);
    std::cout<<"Method GetCurrentState Sucessfully Called\n";
    // if (seteuid(root_uid) != 0) 
    // {
    //     std::cerr << "Error: Failed to restore root privileges!\n";
    //     exit(1);
    // }

    return displayState{serial, monitors, logicalMonitors, properties};
}

int displayManager::setRefreshRateToMax()
{

    displayState currentState = displayManager::getCurrentState();

    uint32_t serial = currentState.serial;

    uint32_t method = 1;

    MonitorStruct& monitor = currentState.monitors[0];

    ConnectorInfo& connectorInfo = monitor.template get<0>();

    std::string connector_name = connectorInfo.template get<0>();

    std::map<std::string, sdbus::Variant> emptyProps;

    auto modes = monitor.template get<1>();

    double maxRefreshRate = 0;
    int32_t currentWidth = 0, currentHeight = 0;
    std::string maxModeID = "";

    for(const auto& mode: modes)
    {
        auto modeProperties = mode.template get<6>();
        bool isCurrent = 0;
        
        if(modeProperties.count("is-current"))
        {
            isCurrent = modeProperties.at("is-current").get<bool>();
        }
        if(isCurrent)
        {
            currentHeight = mode.template get<2>();
            currentWidth = mode.template get<1>();
            break;
        }

    }

    if(currentWidth == 0 || currentHeight == 0) 
    {
        std::cerr<<"Error: Current display mode not found\n";
        return 0;
    }

    for(const auto& mode: modes)
    {
        if(currentHeight == mode.template get<2>() && currentWidth == mode.template get<1>())
        {
            if(maxRefreshRate<mode.template get<3>())
            {
                maxRefreshRate = mode.template get<3>();
                maxModeID = mode.template get<0>();
            }
        }
    }


    ApplyConnectorInfo applyConnectorInfo = sdbus::make_struct(connector_name, maxModeID, emptyProps);
    
    std::vector<ApplyConnectorInfo> connectorInfoArray {applyConnectorInfo};

    const LogicalMonitorStruct& logicalMonitor = currentState.logicalMonitors[0];   

    ApplyLogicalMonitor newLogicalMonitor = sdbus::make_struct
    (
        logicalMonitor.template get<0>(),
        logicalMonitor.template get<1>(),
        logicalMonitor.template get<2>(),
        logicalMonitor.template get<3>(),
        logicalMonitor.template get<4>(),
        connectorInfoArray

    );

    std::vector<ApplyLogicalMonitor> newLogicalMonitorArray {newLogicalMonitor};

    try
    {

        displayConfigProxy->callMethod("ApplyMonitorsConfig")
                .onInterface("org.gnome.Mutter.DisplayConfig")
                .withArguments(serial, method, newLogicalMonitorArray, emptyProps);

        std::cout<<"Refresh Rate sucessfully changed to "<<maxRefreshRate<<"Hz\n\n";
        return 1;
    }
    catch (const sdbus::Error& e) 
    {
        std::cerr << "Error: Failed to apply new layout(For changing the refresh rate): " << e.getMessage() << "\n";
        return 0;
    }
}


int displayManager::setRefreshRateToMin()
{
    displayState currentState = displayManager::getCurrentState();
    uint32_t serial = currentState.serial;
    uint32_t method = 1;

    MonitorStruct& monitor = currentState.monitors[0];
    ConnectorInfo& connectorInfo = monitor.template get<0>();
    std::string connector_name = connectorInfo.template get<0>();
    std::map<std::string, sdbus::Variant> emptyProps;

    auto modes = monitor.template get<1>();

    double minRefreshRate = 1000; 
    int32_t currentWidth = 0, currentHeight = 0;
    std::string minModeID = "";

    for(const auto& mode: modes)
    {
        auto modeProperties = mode.template get<6>();
        bool isCurrent = false;
        
        if(modeProperties.count("is-current"))
        {
            isCurrent = modeProperties.at("is-current").get<bool>();
        }
        if(isCurrent)
        {
            currentWidth = mode.template get<1>();
            currentHeight = mode.template get<2>();
            break; 
        }
    }

    if(currentWidth == 0 || currentHeight == 0) 
    {
        std::cerr<<"Error: Current mode not found\n\n";
        return 0;
    }
    for(const auto& mode: modes)
    {
        if(currentHeight == mode.template get<2>() && currentWidth == mode.template get<1>())
        {
            // Flipped to > to capture the lowest value
            if(minRefreshRate > mode.template get<3>()) 
            {
                minRefreshRate = mode.template get<3>();
                minModeID = mode.template get<0>();
            }
        }
    }

    ApplyConnectorInfo applyConnectorInfo = sdbus::make_struct(connector_name, minModeID, emptyProps);
    std::vector<ApplyConnectorInfo> connectorInfoArray {applyConnectorInfo};

    const LogicalMonitorStruct& logicalMonitor = currentState.logicalMonitors[0];   

    ApplyLogicalMonitor newLogicalMonitor = sdbus::make_struct(
        logicalMonitor.template get<0>(),
        logicalMonitor.template get<1>(),
        logicalMonitor.template get<2>(),
        logicalMonitor.template get<3>(),
        logicalMonitor.template get<4>(),
        connectorInfoArray
    );

    std::vector<ApplyLogicalMonitor> newLayoutArray {newLogicalMonitor};

    try
    {
        displayConfigProxy->callMethod("ApplyMonitorsConfig")
                .onInterface("org.gnome.Mutter.DisplayConfig")
                .withArguments(serial, method, newLayoutArray, emptyProps); 

        std::cout << "Refresh Rate successfully changed to " << minRefreshRate << "Hz\n\n";
        return 1;
    }
    catch (const sdbus::Error& e) 
    {
        std::cerr << "Error: Failed to apply new layout (Min): " << e.getMessage() << "\n";
        return 0;
    }
}