# MondgoDB for KOS build

## Brief

There are 2 ways to build and run MongoDB under KOS:

- using host environment;
- using docker.

Docker is much easier to use because it takes responsibility for preparing the
build environment: KOS SDK, python, pip, etc.  
It will also allow you to keep your host environment clean.

## Prerequisites

Before you begin, ensure that you have met the following requirements:

### Build in host environment

- You have Debian GNU/Linux "Buster" 10 x64 or above;
- You have installed the latest version of [KasperskyOS Community Edition][1];
- You met the [MongoDB build](../docs/building.md) requirements;

### Build using docker

- You have installed docker;
- You have successfully built a docker image with KOS MongoDB build environment;

To build the Docker image, run [docker/build-image.sh](docker/build-image.sh).  
To create and run a Docker container from the image created in the previous
step, run
[docker/create-and-run-container.sh](docker/create-and-run-container.sh).

## Build

Building in a host or docker environment is the same.

Run `make help` to get a list of all available targets.

### QEMU image

1. `cd <repo_root>/kos/mongod/`;
2. `make compile-mongod`;
3. `make qemubuild`;
4. `make qemurun`;
5. **mongod** is running on KOS QEMU and listening on localhost:`MONGOD_PORT`
port (default 27018).

### HW (RaspberryPi) image

1. `cd <repo_root>/kos/mongod/`;
2. `make compile-mongod`;
3. `make hwbuild`;  
   Resulting image path: *<target_arch>/einit/kos-image*;
4. [Prepare][2] SD Card for RaspberryPi;
5. Copy the image to the SD card;
6. Turn on your Raspberry Pi. Communication is implemented via UART;
7. Using a COM terminal (minicom or otherwise), run the following command after starting u-boot:

```
U-Boot> ext4load mmc 0:<partition_num> 0x200000 kos-image
...
U-Boot> bootelf
```


[1]: https://os.kaspersky.com/development/download/
[2]: https://support.kaspersky.com/help/KCE/1.1/ru-RU/preparing_sd_card_rpi.htm

