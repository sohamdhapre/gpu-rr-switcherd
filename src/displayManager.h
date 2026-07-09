#pragma once

#include <sdbus-c++/sdbus-c++.h>
#include <string>

//Connector Info: (connector_name, vendor, product, serial)
using ConnectorInfo = sdbus::Struct<std::string, std::string, std::string, std::string>;

// Mode Info: (mode_id, width, height, refresh_rate, preferred_scale, supported_scales, properties)
using ModeInfo = sdbus::Struct<std::string, int32_t, int32_t, double, double, std::vector<double>, std::map<std::string, sdbus::Variant>>;

//Monitor Object: (ConnectorInfo, array_of_modes, monitor_properties)
using MonitorStruct = sdbus::Struct<ConnectorInfo, std::vector<ModeInfo>, std::map<std::string, sdbus::Variant>>;

//Logical Monitor Object (X, Y, Scaling,Transform, isPrimary, ConnectorInfo, Properties)
using LogicalMonitorStruct = sdbus::Struct<int32_t, int32_t, double, uint32_t, bool, std::vector<ConnectorInfo>, std::map<std::string, sdbus::Variant>>;

using displayState = struct 
{
    uint32_t serial = 0;
    std::vector<MonitorStruct> monitors;
    std::vector<LogicalMonitorStruct> logicalMonitors;
    std::map<std::string, sdbus::Variant> properties;
};

// (connector_name, target_mode_id, properties)
using ApplyConnectorInfo = sdbus::Struct<std::string, std::string, std::map<std::string, sdbus::Variant>>;

// (x, y, scale, transform, primary, list_of_connectors)
using ApplyLogicalMonitor = sdbus::Struct<int32_t, int32_t, double, uint32_t, bool, std::vector<ApplyConnectorInfo>>;


class displayManager
{

    public:
        displayManager();

        double getCurrentRefreshRate();
        int setRefreshRateToMax();
        int setRefreshRateToMin();
        displayState getCurrentState();



    private:
        std::unique_ptr<sdbus::IProxy> displayConfigProxy;
        std::unique_ptr<sdbus::IConnection> connection;
        int initDBus();


        




};