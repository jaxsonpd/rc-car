# Install

This document details the installation process for the toolchain on wsl

# Installing WSL

Source: https://learn.microsoft.com/en-us/windows/wsl/install

Open a powershell prompt as an admimastrator and run:

```
wsl --install
```
This installs ubuntu.

Restart the terminal and now ubuntu should be in the drop down. Let it install then set user name and password.

Once this is completed you can then ensure it is up to date using:

```
sudo apt update && sudo apt upgrade
```

# USB pass through

Source: https://learn.microsoft.com/en-us/windows/wsl/connect-usb

1. Install usbipd (download the msi)
2. Attach usb device to the windows machine
3. Run 
```
usbipd list
```
to list comands
4. bind the usb device that should be shared (this makes it available to wsl)
```
usbipd bind --busid 4-4
```
5. Attach the usb device to wsl
```
usbipd attach --wsl --busid <busid>
```
6. Then use:
```
lsusb
```
To list the devices attached to the ubuntu installation

# Toolchain

The toolchain can then be downloaded using the guide found at:

https://eng-git.canterbury.ac.nz/wacky-racers/wacky-racers/-/raw/master/doc/guide/guide.pdf?inline=true
