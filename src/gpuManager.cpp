#include "gpuManager.h"

const char* blacklistFileContent = R"(#File created by gpu-rr-switcher
blacklist nouveau
blacklist nova_core
blacklist nova_drm
blacklist nvidia
blacklist nvidia_drm
blacklist nvidia_uvm
blacklist nvidia_modeset
blacklist nvidia_current
blacklist nvidia_current_drm
blacklist nvidia_current_uvm
blacklist nvidia_current_modeset
blacklist i2c_nvidia_gpu
alias nouveau off
alias nova_core off
alias nova_drm off
alias nvidia off
alias nvidia_drm off
alias nvidia_uvm off
alias nvidia_modeset off
alias nvidia_current off
alias nvidia_current_drm off
alias nvidia_current_uvm off
alias nvidia_current_modeset off
alias i2c_nvidia_gpu off)";


const char* blacklistFilePath = "/etc/modprobe.d/blacklist-nvidia.conf";

const char* udevFilePath = "/etc/udev/rules.d/50-remove-nvidia.rules";

const char* udevFileContent = R"(#File created by gpu-rr-switcher
ACTION=="add", SUBSYSTEM=="pci", ATTR{vendor}=="0x10de", ATTR{class}=="0x0c0330", ATTR{power/control}="auto", ATTR{remove}="1"

ACTION=="add", SUBSYSTEM=="pci", ATTR{vendor}=="0x10de", ATTR{class}=="0x0c8000", ATTR{power/control}="auto", ATTR{remove}="1"

ACTION=="add", SUBSYSTEM=="pci", ATTR{vendor}=="0x10de", ATTR{class}=="0x040300", ATTR{power/control}="auto", ATTR{remove}="1"

ACTION=="add", SUBSYSTEM=="pci", ATTR{vendor}=="0x10de", ATTR{class}=="0x03[0-9]*", ATTR{power/control}="auto", ATTR{remove}="1"
)";

const char* modesetFilePath = "/etc/modprobe.d/nvidia.conf";

const char* modesetFileContent = R"(#File created by gpu-rr-switcher

options nvidia-drm modeset=1
options nvidia "NVreg_DynamicPowerManagement=0x02"
options nvidia NVreg_UsePageAttributeTable=1 NVreg_InitializeSystemMemoryAllocations=0
)";

const char* udevPmFilePath = "/etc/udev/rules.d/80-nvidia-pm.rules";

const char* udevPmFileContent = R"(#File created by gpu-rr-switcher

ACTION=="add", SUBSYSTEM=="pci", ATTR{vendor}=="0x10de", ATTR{class}=="0x0c0330", ATTR{remove}="1"

ACTION=="add", SUBSYSTEM=="pci", ATTR{vendor}=="0x10de", ATTR{class}=="0x0c8000", ATTR{remove}="1"

ACTION=="add", SUBSYSTEM=="pci", ATTR{vendor}=="0x10de", ATTR{class}=="0x040300", ATTR{remove}="1"

ACTION=="bind", SUBSYSTEM=="pci", ATTR{vendor}=="0x10de", ATTR{class}=="0x030000", TEST=="power/control", ATTR{power/control}="auto"
ACTION=="bind", SUBSYSTEM=="pci", ATTR{vendor}=="0x10de", ATTR{class}=="0x030200", TEST=="power/control", ATTR{power/control}="auto"

ACTION=="unbind", SUBSYSTEM=="pci", ATTR{vendor}=="0x10de", ATTR{class}=="0x030000", TEST=="power/control", ATTR{power/control}="on"
ACTION=="unbind", SUBSYSTEM=="pci", ATTR{vendor}=="0x10de", ATTR{class}=="0x030200", TEST=="power/control", ATTR{power/control}="on"
)";

void gpuManager::promptRestart(Mode mode) 
{
    std::cout<<"promptRestart called\n";

    std::thread([mode]()
    {
        std::string command;
        if(mode == Mode::integrated)
        {
        command = "sudo -u $(id -un 1000) env XDG_RUNTIME_DIR=/run/user/1000 DBUS_SESSION_BUS_ADDRESS=unix:path=/run/user/1000/bus notify-send -w -u critical -A 'reboot=Restart Now' 'AC power disabled' 'A reboot is required to disable the dGPU.' 2>&1";
        }
        else if (mode == Mode::hybrid)
        {
        command = "sudo -u $(id -un 1000) env XDG_RUNTIME_DIR=/run/user/1000 DBUS_SESSION_BUS_ADDRESS=unix:path=/run/user/1000/bus notify-send -w -u critical -A 'reboot=Restart Now' 'AC power enabled' 'A reboot is required to enable the dGPU.' 2>&1";
        }
        std::array<char, 128> buffer;
        std::string result;
        
        std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(command.c_str(), "r"), pclose);
        
        if (!pipe) 
        {
            std::cerr << "Failed to open pipe for notification.\n";
            return;
        }
        
        while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) 
        {
            result += buffer.data();
        }
        std::cout << "[Notification Output] " << result << "\n";
        
        if (result.find("reboot") != std::string::npos) 
        {
            std::cout << "[Action] User clicked Restart. Rebooting system...\n";
            std::cout<<"Reboot selected\n";
            

            std::system("reboot now");
            return;
        }

        else
        {
            return;
        }

    }).detach();
}

int gpuManager::enableNvidiaPersistanceService()
{
    const char* command = "systemctl enable nvidia-persistenced.service > /dev/null 2>&1";

    int result = std::system(command);

    if(!result)
    {
        std::cout<<"Sucessfully enabled Nvidia Persistance Service\n";
        return 1;
    }
    else
    {
        std::cerr<<"Error enabling Nvidia Persistance Service\n";
        return 0;
    }
}

int gpuManager::disableNvidiaPersistanceService()
{
    const char* command = "systemctl disable nvidia-persistenced.service > /dev/null 2>&1";

    int result = std::system(command);

    if(!result)
    {
        std::cout<<"Sucessfully disabled Nvidia Persistance Service\n";
        return 1;
    }
    else
    {
        std::cerr<<"Error disabling Nvidia Persistance Service\n";
        return 0;
    }
}

int gpuManager::rebuild()
{
    const char* command = nullptr;

    if (fs::exists("/ostree") || fs::exists("/sysroot/ostree")) 
    {
        std::cout << "Rebuilding the initramfs with rpm-ostree...\n";
        command = "rpm-ostree initramfs --enable --arg=--force";
    }
    else if (fs::exists("/etc/debian_version")) 
    {
        command = "update-initramfs -u -k all";
    }
    else if (fs::exists("/etc/redhat-release") || fs::exists("/usr/bin/zypper")) 
    {
        command = "dracut --force --regenerate-all";
    }
    else if (fs::exists("/usr/lib/endeavouros-release") && fs::exists("/usr/bin/dracut")) 
    {
        command = "dracut-rebuild";
    }
    else if (fs::exists("/etc/altlinux-release")) 
    {
        command = "make-initrd";
    }
    else if (fs::exists("/etc/arch-release")) 
    {
        command = "mkinitcpio -P";
    }

    if (command != nullptr) 
    {
        std::cout << "Executing: " << command << "\n";
        return std::system(command);
    }

    std::cerr << "Unsupported OS layout. Cannot determine initramfs rebuild command.\n";
    return 0; 
}

int gpuManager::setToIntegratedMode()
{

    clean();

    disableNvidiaPersistanceService();

    std::ofstream blacklistFIle (blacklistFilePath);

    if(blacklistFIle.is_open())
    {
        blacklistFIle<<blacklistFileContent<<'\n';
        blacklistFIle.close();
    }
    else
    {
        std::cerr<<"Error opening or creating this file: "<<blacklistFilePath<<'\n';
        return 0;
    }

    std::ofstream udevFile = std::ofstream(udevFilePath);

    if(udevFile.is_open())
    {
        udevFile<<udevFileContent<<'\n';
        udevFile.close();
    }
    else
    {
        std::cerr<<"Error opening or cresting this file: "<<udevFilePath<<'\n';
        return 0;
    }

    
    rebuild();
    return 1;
}

int gpuManager::setToHybridMode()
{
    clean();

    enableNvidiaPersistanceService();

    std::ofstream modesetFile(modesetFilePath);

    if(modesetFile.is_open())
    {
        modesetFile<<modesetFileContent<<'\n';
        modesetFile.close();
    }
    else
    {
        std::cerr<<"Error opening or creating this file: "<<modesetFilePath<<'\n';
        return 0;
    }

    std::ofstream udevPmFile (udevPmFilePath);

    if(udevPmFile.is_open())
    {
        udevPmFile<<udevPmFileContent<<'\n';
        udevPmFile.close();
    }
    else
    {
        std::cerr<<"Error opening or creating this file: "<<udevPmFilePath<<'\n';
        return 0;
    }

    rebuild();
    return 1;

}

int gpuManager::clean()
{
    fs::remove(blacklistFilePath);
    fs::remove(udevFilePath);
    fs::remove(modesetFilePath);
    fs::remove(udevPmFilePath);
    
    fs::remove("/etc/X11/xorg.conf"); 

    std::cout << "Cleaned up previous configuration files.\n";
    return 1;
}

Mode gpuManager::getGPUMode()
{
    if (fs::exists("/sys/module/nvidia")) 
    {
        return Mode::hybrid;
    }
    
    return Mode::integrated;
}