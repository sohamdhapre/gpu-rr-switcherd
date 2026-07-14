Name:           gpu-rr-switcherd
Version:        1.0.0
Release:        1%{?dist}
Summary:        Early-boot GPU management and dynamic refresh rate Wayland daemon

License:        MIT
URL:            https://github.com/sohamdhapre/gpu-rr-switcherd
Source0:        %{url}/archive/refs/tags/v%{version}.tar.gz

BuildRequires:  gcc-c++
BuildRequires:  sdbus-c++-devel
BuildRequires:  systemd-devel
Requires:       sdbus-c++
Requires:       systemd-libs
Requires:       dracut
Requires:       libnotify

%description
A lightweight C++ system daemon designed to optimize power consumption on Linux laptops running GNOME Wayland. It intercepts the boot sequence via dracut to sever Nvidia GPU power on battery, and dynamically adjusts display refresh rates based on AC power states.

%prep
%setup -q

%build
g++ src/main.cpp src/displayManager.cpp src/gpuManager.cpp src/powerMonitor.cpp -std=c++17 -lsdbus-c++ -ludev -o gpu-rr-switcherd

%install
rm -rf $RPM_BUILD_ROOT

# Install the binary
install -D -m 0755 gpu-rr-switcherd %{buildroot}/usr/bin/gpu-rr-switcherd

# Install the systemd service
install -D -m 0644 gpu-rr-switcherd.service %{buildroot}/usr/lib/systemd/system/gpu-rr-switcherd.service

# Install the dracut hooks
install -d %{buildroot}/usr/lib/dracut/modules.d/99gpu-switcher
install -m 0755 check-power.sh %{buildroot}/usr/lib/dracut/modules.d/99gpu-switcher/check-power.sh
install -m 0755 module-setup.sh %{buildroot}/usr/lib/dracut/modules.d/99gpu-switcher/module-setup.sh

%files
/usr/bin/gpu-rr-switcherd
/usr/lib/systemd/system/gpu-rr-switcherd.service
/usr/lib/dracut/modules.d/99gpu-switcher/check-power.sh
/usr/lib/dracut/modules.d/99gpu-switcher/module-setup.sh

%post
%systemd_post gpu-rr-switcherd.service
echo "Installation complete. Please run 'sudo dracut --force' to update your initramfs."

%preun
%systemd_preun gpu-rr-switcherd.service

%postun
%systemd_postun_with_restart gpu-rr-switcherd.service
