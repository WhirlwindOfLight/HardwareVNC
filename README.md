# HardwareVNC
This program is designed to help control VMs created with [GPU passthrough](https://wiki.archlinux.org/title/PCI_passthrough_via_OVMF), since those VMs can't easily use traditional remoting solutions that involve attaching a server directly to a virtual GPU. As the name implies, this program works by using hardware to control the VM, by using a capture card connected to the passed-through GPU to see the display, and by using a TCP connection to [another program](https://github.com/WhirlwindOfLight/VM-Controller) to control the keyboard and mouse. This program is only designed for use on Linux, so instructions will assume you are on Linux.

## Installing
Binaries are currently not officially distributed for this program, so it will need to be compiled from source.

## Building from Source
1. Download the dependencies using your system package manager:
    * make
    * g++
    * yaml-cpp
    * opencv
    * libvncserver
2. Navigate to where you want to store the source code
3. Clone the repository with `git clone https://github.com/WhirlwindOfLight/HardwareVNC.git`
4. Enter the directory with `cd HardwareVNC`
5. Build the program with `make`

## Usage
1. Make sure [the controller program](https://github.com/WhirlwindOfLight/VM-Controller) is running and a capture card is connected to the desired GPU
2. Create a config file following the format in `templates/hardwareVncServer.conf`
3. Either directly or using a systemd service, run the program: `hardwareVncServer <config file>`
4. Connect to the server with any VNC client software using `listen-port` as defined in your config file

## Licensing
All past, present, and future code in this repository is licensed with GPLv3 as of Febuary 17, 2026 (see [LICENSE](LICENSE) for more details). In addition, when compiled, the code links against the following libraries with their associated licenses:
* [yaml-cpp](https://github.com/jbeder/yaml-cpp) (MIT)
* [opencv](https://github.com/opencv/opencv) (Apache-2.0)
* [libvncserver](https://github.com/LibVNC/libvncserver) (GPLv2+)