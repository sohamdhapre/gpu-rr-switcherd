#pragma once
#include <string>
#include <iostream>
#include <cstdlib>
#include <filesystem>
namespace fs = std::filesystem;

#include <fstream>

class gpuManager
{
   public:
        static int setToIntegratedMode();
        static int setToHybridMode();

    private:
        static int enableNvidiaPersistanceService();
        static int disableNvidiaPersistanceService();
        static int clean();
        static int rebuild();

};