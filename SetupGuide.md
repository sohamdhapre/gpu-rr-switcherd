Setup & Installation Guide: GPU & Refresh Rate Switcher (gpu-rr-switcherd)

This guide outlines how to deploy the gpu-rr-switcherd system daemon. This utility optimizes power consumption on Wayland laptops by intercepting the boot sequence via dracut to sever Nvidia GPU power on battery, while dynamically adjusting display refresh rates based on AC power states.

Choose the installation method that best matches your Linux distribution and workflow.

Method 1: Arch Linux (AUR)

For Arch Linux and Arch-based distributions (Manjaro, EndeavourOS), the daemon is packaged natively using a PKGBUILD.

1. Clone the repository and build the package:

git clone [https://github.com/sohamdhapre/gpu-rr-switcherd.git](https://github.com/sohamdhapre/gpu-rr-switcherd.git)
cd gpu-rr-switcherd
makepkg -si


Command Breakdown:

git clone: Downloads the latest version of the repository to your local machine.

cd: Navigates into the newly downloaded project folder.

makepkg -si: An Arch Linux specific command that reads the PKGBUILD file, automatically downloads missing dependencies (-s), compiles the C++ code, and installs the final package directly into your system (-i).

2. Enable the service and update the initramfs:

sudo systemctl enable --now gpu-rr-switcherd.service
sudo dracut --force


Method 2: Fedora / RHEL (RPM Build)

For Fedora, Red Hat, and CentOS, the daemon can be built into a native .rpm package using the provided .spec file.

1. Install the RPM build tools and project dependencies:

sudo dnf install rpmdevtools rpm-build spectool gcc-c++ systemd-devel sdbus-cpp-devel


2. Prepare the build environment and compile the RPM:

rpmdev-setuptree
spectool -g -R packaging/fedora/gpu-rr-switcherd.spec
rpmbuild -ba packaging/fedora/gpu-rr-switcherd.spec


Command Breakdown:

rpmdev-setuptree: Creates the standard ~/rpmbuild/ directory structure required for assembling RPM packages safely in your home folder.

spectool -g -R: Reads the project's .spec file and automatically downloads the official release source code .tar.gz from GitHub into your build tree.

rpmbuild -ba: Executes the instructions in the .spec file to compile the source code and generate the final installable .rpm file.

3. Install the generated package and enable the service:

sudo dnf install ~/rpmbuild/RPMS/x86_64/gpu-rr-switcherd-1.0.0-1.*.rpm
sudo systemctl enable --now gpu-rr-switcherd.service
sudo dracut --force


Method 3: Manual Installation (Developers & Ubuntu/Debian)

This section is for developers modifying the source code or users on distributions lacking a native package manager configuration.

1. Deploy the Early-Boot Initramfs Hook

Run these commands in the directory containing check-power.sh and module-setup.sh:

sudo mkdir -p /usr/lib/dracut/modules.d/99gpu-switcher
sudo cp check-power.sh module-setup.sh /usr/lib/dracut/modules.d/99gpu-switcher/
sudo chmod +x /usr/lib/dracut/modules.d/99gpu-switcher/*.sh
sudo dracut --force


Command Breakdown:

mkdir -p: Creates the required module directory structure if it doesn't exist.

cp: Copies your hook scripts into the newly created dracut module directory.

chmod +x: Makes the scripts executable, which is required for dracut to run them.

dracut --force: Rebuilds the system's initial ramdisk image to include the new scripts.

2. Compile the Daemon

Run this command in the directory containing your .cpp and .h source code files:

g++ main.cpp displayManager.cpp gpuManager.cpp powerMonitor.cpp -std=c++17 -lsdbus-c++ -ludev -o gpu-rr-switcherd


Command Breakdown:

g++: Invokes the GNU C++ compiler.

*.cpp files: The source code files being compiled together into a single program.

-std=c++17: Instructs the compiler to enforce the C++17 standard, enabling modern language features.

-lsdbus-c++: Links the sdbus-c++ library, required for the daemon to communicate with system services over D-Bus.

-ludev: Links the libudev library, allowing the daemon to actively monitor hardware device events (like plugging in the AC adapter).

-o gpu-rr-switcherd: Specifies the final output filename for the compiled binary executable.

3. Install the Binary

Move the newly compiled executable to your system's binaries folder:

sudo mv gpu-rr-switcherd /usr/local/bin/
sudo chmod +x /usr/local/bin/gpu-rr-switcherd


Command Breakdown:

mv: Moves the compiled binary into /usr/local/bin/, making it globally accessible as a system command.

chmod +x: Ensures the binary has the correct execution permissions needed to run as a system daemon.

4. Create the Systemd Background Service

Create a new systemd unit file by running:

sudo nano /etc/systemd/system/gpu-rr-switcherd.service


Paste the following configuration exactly as formatted:

[Unit] 
Description=GPU & Refresh Rate Switcher Daemon 
After=multi-user.target display-manager.service

[Service] 
Type=simple
ExecStart=/usr/local/bin/gpu-rr-switcherd
Restart=on-failure 
RestartSec=5
User=root

[Install]
WantedBy=multi-user.target


Save and exit your text editor (in Nano: Ctrl+O, Enter, Ctrl+X).

5. Enable and Start the Daemon

Finally, register and launch the newly created service:

sudo systemctl daemon-reload
sudo systemctl enable --now gpu-rr-switcherd.service


Command Breakdown:

daemon-reload: Tells systemd to scan for and read the new .service unit file we just created.

enable --now: The --now flag is a handy shortcut that configures the service to start automatically on system boot and immediately starts the service in the current session without requiring a reboot.
