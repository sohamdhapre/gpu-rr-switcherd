#!/bin/sh

AC_CONNECTED=0

for supply in /sys/class/power_supply/*; do
    if [ -f "$supply/online" ] && [ "$(cat $supply/online)" = "1" ]; then
        AC_CONNECTED=1
        break
    fi
done

if [ "$AC_CONNECTED" = "0" ]; then
    echo "GPU-Switcher: Battery boot detected. Disabling discrete GPU..." > /dev/kmsg
    
    cat << 'EOF' > /etc/modprobe.d/blacklist-nvidia.conf

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
alias i2c_nvidia_gpu off
EOF
    
    cat << 'EOF' > /etc/udev/rules.d/50-remove-nvidia.rules

ACTION=="add", SUBSYSTEM=="pci", ATTR{vendor}=="0x10de", ATTR{class}=="0x0c0330", ATTR{power/control}="auto", ATTR{remove}="1"
ACTION=="add", SUBSYSTEM=="pci", ATTR{vendor}=="0x10de", ATTR{class}=="0x0c8000", ATTR{power/control}="auto", ATTR{remove}="1"
ACTION=="add", SUBSYSTEM=="pci", ATTR{vendor}=="0x10de", ATTR{class}=="0x040300", ATTR{power/control}="auto", ATTR{remove}="1"
ACTION=="add", SUBSYSTEM=="pci", ATTR{vendor}=="0x10de", ATTR{class}=="0x03[0-9]*", ATTR{power/control}="auto", ATTR{remove}="1"
EOF

else
    echo "GPU-Switcher: AC power boot detected. Allowing hybrid mode graphics..." > /dev/kmsg
  fi
