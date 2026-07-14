This is a lightweight C++ system daemon designed to optimize power consumption Linux laptops.
Features:
>Early-Boot GPU Management: During Boot the script check-power.sh chacks if the laptop is plugged in and if it is not, temporarly modifies the initramfs, creating the files blacklist-nvidia.conf and 50-remove-nvidia.rules , to block the kernel from loading the drivers for the nvidia dGPU, and severing the power to the dGPU.
>If the AC charger is removed or plugged in while the laptop is running, the daemon changes the refresh rate of the display to maximum supported (on AC Power) or minimum supported.
And it prompts the user for a reboot to disable or enable the dGPU.
