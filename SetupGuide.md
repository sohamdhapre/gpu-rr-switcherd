# GPU & REFRESH RATE SWITCHER (gpu-rr-switcherd) SETUP GUIDE

This guide outlines the steps to deploy the early-boot hook, compile the daemon,

and configure the systemd service for the GPU & Refresh Rate Switcher.

## STEP 1: Deploy the Early-Boot Initramfs Hook

Run these commands in the directory containing `check-power.sh` and `module-setup.sh`:

```
`sudo mkdir -p /usr/lib/dracut/modules.d/99gpu-switcher`

`sudo cp check-power.sh module-setup.sh /usr/lib/dracut/modules.d/99gpu-switcher/`

`sudo chmod +x /usr/lib/dracut/modules.d/99gpu-switcher/\*.sh`
```

Command Breakdown:

- mkdir -p: Creates the required module directory structure if it doesn't exist.

- cp: Copies your hook scripts into the newly created dracut module directory.

- chmod +x: Makes the scripts executable, which is required for dracut to run them.

Next, rebuild the initramfs.

Note: If you are on Ubuntu/Debian or Arch, ensure `dracut` is installed and used

to generate your boot image, as this module structure is dracut-specific.` `

```
`  sudo dracut --force`


## STEP 2: Compile the Daemon

Run this command in the directory containing the `.cpp` and `.h` source code files:

```
`g++ main.cpp displayManager.cpp gpuManager.cpp powerMonitor.cpp -std=c++17 -lsdbus-c++ -ludev -o gpu-rr-switcherd`
```

Command Breakdown:

- g++: Invokes the GNU C++ compiler.

- The source files being compiled together.

- -std=c++17: Enforces the C++17 standard for modern feature compatibility.

- -lsdbus-c++ / -ludev: Links the required D-Bus and udev libraries.

- -o: Specifies the output filename (`gpu-rr-switcherd`) for the compiled binary.

## STEP 3: Install the Binary

Move the newly compiled executable to your system's binaries folder:

```
`sudo mv gpu-rr-switcherd /usr/local/bin/`

`sudo chmod +x /usr/local/bin/gpu-rr-switcherd`
```

Command Breakdown:

- mv: Moves the compiled binary into `/usr/local/bin/` making it globally accessible.

- chmod +x: Ensures the binary has execution permissions for the system daemon.

## STEP 4: Create the Systemd Background Service

Create a new systemd unit file by running:

```
`sudo nano /etc/systemd/system/gpu-rr-switcherd.service`
```

Paste the following configuration exactly as formatted:

```
`\[Unit\] `

`Description=GPU & Refresh Rate Switcher Daemon `

`After=multi-user.target display-manager.service`


`\[Service\] `

`Type=simple`

`ExecStart=/usr/local/bin/gpu-rr-switcherd`

`Restart=on-failure `

`RestartSec=5`

`User=root`


`\[Install\]`

`WantedBy=multi-user.target`
```

Save (Ctrl+O, Enter) and exit (Ctrl+X).

## STEP 5: Enable and Start the Daemon

Finally, register and start the newly created service:

```
`sudo systemctl daemon-reload`

`sudo systemctl enable gpu-rr-switcherd.service`

`sudo systemctl start gpu-rr-switcherd.service`
```

Command Breakdown:

- daemon-reload: Tells systemd to read the new unit file we just created.

- enable: Configures the service to start automatically on system boot.

- start: Immediately launches the daemon without needing a reboot.

