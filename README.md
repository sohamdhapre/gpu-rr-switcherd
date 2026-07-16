This is a lightweight C++ system daemon designed to optimize power consumption Linux laptops.
Features:
>Early-Boot GPU Management: During boot, the check-power.sh dracut hook evaluates the AC power state. If the laptop is on battery, it generates blacklist-nvidia.conf and 50-remove-nvidia.rules within the live initramfs environment. This blocks the kernel from loading the Nvidia drivers and instructs the PCI subsystem to sever power to the dGPU before the desktop environment even initializes.

>Dynamic Runtime Adjustments: If the AC charger is connected or disconnected while the laptop is running in an active Wayland session, the C++ daemon instantly intercepts the power state change. It switches the display to the maximum supported refresh rate (on AC) or minimum supported refresh rate. It then prompts the user via a desktop notification to reboot, allowing the early-boot hook to properly toggle the dGPU hardware state.
