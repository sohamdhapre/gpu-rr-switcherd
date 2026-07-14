#pragma once
#include <string>
#include <iostream>
#include <cstdlib>
#include <filesystem>
#include <thread>
#include <array>
namespace fs = std::filesystem;

#include <fstream>

enum class Mode {integrated =0, hybrid =1, notAvailable = 3};

class gpuManager
{
   public:
        static int setToIntegratedMode();
        static int setToHybridMode();
        static void promptRestart(Mode);
        static Mode getGPUMode();

    private:
        static int enableNvidiaPersistanceService();
        static int disableNvidiaPersistanceService();
        static int clean();
        static int rebuild();

};

