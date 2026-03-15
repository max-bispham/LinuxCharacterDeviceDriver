# SHT30 Linux Kernel Driver

A combined I2C + character device driver for the Adafruit SHT30 temperature and humidity sensor, written as a Linux kernel module for the Raspberry Pi.

## Overview

This driver registers the SHT30 sensor as a character device, allowing userspace applications to read temperature and humidity data via standard file I/O. It demonstrates the pattern of combining the Linux I2C driver subsystem with the character device subsystem in a single kernel module.

## Architecture
```
userspace app
    ↓ read(fd)
character device (/dev/sht30)
    ↓ i2c_master_send/recv
I2C driver (i2c_driver)
    ↓
SHT30 sensor (0x44 on i2c1)
```

## Hardware

- Raspberry Pi (tested on kernel 6.12.62+rpt-rpi-v8)
- Adafruit SHT30 temperature and humidity sensor

### Wiring

| Pi Pin | Pi Function | SHT30 |
|--------|-------------|-------|
| Pin 1  | 3.3V        | VCC   |
| Pin 3  | SDA (GPIO2) | SDA   |
| Pin 5  | SCL (GPIO3) | SCL   |
| Pin 6  | GND         | GND   |

## Prerequisites

Enable I2C on the Pi by adding to `/boot/config.txt`:
```
dtparam=i2c_arm=on
```

Install kernel headers:
```bash
sudo apt install raspberrypi-kernel-headers
```

## Building
```bash
make
```

## Device Tree Overlay

Compile and load the overlay:
```bash
dtc -@ -I dts -O dtb -o sht30.dtbo sht30.dts
sudo dtoverlay sht30.dtbo
```

To make it permanent:
```bash
sudo cp sht30.dtbo /boot/overlays/
echo "dtoverlay=sht30" | sudo tee -a /boot/config.txt
```

## Loading the Driver
```bash
sudo insmod sht30.ko
dmesg | tail  # should show "sht30 - probe called"
```

## Unloading
```bash
sudo rmmod sht30
```

## Concepts Demonstrated

- Linux I2C driver model (`i2c_driver`, `probe`, `remove`)
- Character device registration (`register_chrdev`, `file_operations`)
- Device tree overlays for hardware description
- Linking kernel subsystems via `private_data`
- Userspace/kernel boundary (`copy_to_user`, `__user`)

