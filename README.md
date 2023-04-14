# MongoDB (v6.2) for KasperskyOS

This is a fork of [MongoDB project][1] adapted for usage with KasperskyOS. For
more information about the target OS, please refer to [KasperskyOS Community
Edition][2].

For general information on using MongoDB, its features and so on, please see
the [MongoDB website][3].

Please refer to the original MongoDB (parent) [README.md][4] (also available in
the current repository as [README.mongo.md](./README.mongo.md)) for more
details not related to this fork.

## How to build (Linux host or using Docker)

### Brief

There are 2 ways to build and run MongoDB under KOS:

- using host environment;
- using docker.

Docker is much easier to use because it takes responsibility for preparing the
build environment: KOS SDK, python, pip, etc.  
It will also allow you to keep your host environment clean.

### Prerequisites

Before you begin, ensure that you have met the following requirements:

#### Build in host environment

- You have Debian GNU/Linux "Buster" 10 x64 or above;
- You have installed the latest version of [KasperskyOS Community Edition][5];
- You met the [MongoDB build](./docs/building.md) requirements;

#### Build using docker

- You have installed docker;
- You have successfully built a docker image with KOS MongoDB build environment;

To build the Docker image, run
[kos/docker/build-image.sh](kos/docker/build-image.sh).  
To create and run a Docker container from the image created in the previous
step, run
[kos/docker/create-and-run-container.sh](kos/docker/create-and-run-container.sh).

### Build

Building in a host or docker environment is the same.

Run `make help` to get a list of all available targets.

#### QEMU image

1. `cd <repo_root>/kos/src/`;
2. `make compile-mongod`;
3. `make qemubuild`;
4. `make qemurun`;
5. **mongod** is running on KOS QEMU and listening on localhost:`MONGOD_PORT`
port (default 27018).

#### HW (RaspberryPi) image

1. `cd <repo_root>/kos/src/`;
2. `make compile-mongod`;
3. `make hwbuild`;  
   Resulting image path: *<target_arch>/einit/kos-image*;
4. [Prepare][6] SD Card for RaspberryPi;
5. Copy the image to the SD card;
6. Turn on your Raspberry Pi. Communication is implemented via UART;
7. Using a COM terminal (minicom or otherwise), run the following command after
starting u-boot (assuming that your `num` partition is formatted to ext4):

```
U-Boot> ext4load mmc 0:<partition_num> 0x200000 kos-image
...
U-Boot> bootelf 0x200000

```

## Directory structure

- *kos* - directory with KOS related stuff;
  - *docker* - Docker files and helper scripts;
    - *Dockerfile* - dockerfile to create a Docker image with KOS CE SDK
      installed and MongoDB build requirements;
    - *build-image.sh* - example command to create a Docker image;
    - *create-and-run-container.sh* - example command to create and run a
      Docker container from a previously created image;
  - *src* - src code of KOS entities, rules for building an image;
    - *einit*, *mongod* - sources and CMake build rules for corresponding
      entities;
    - *resources* - description files for all the entities, additional files to
      place in the resulting image;
      - *edl* - entities *.edl description files;
      - *image* - additional files to place in the resulting image;
    - *CMakeLists.txt* - project-level CMake build rules;
    - *Makefile* - main Makefile to configure/build/run MongoDB project for KOS;
  - *patches* - optional patches for MongoDB KOS;

## Known issues and limitations

**Currently, only the mongod (server daemon) build is supported for KOS**.

1. **IPv6** is not yet supported at the moment, as it's not available in KOS for
now;
2. Subprocess creation isn't available in KOS for now;
3. Only static build of MongoDB is supported (dynamic libraries are not
supported by KOS);
4. Mongo **mozjs** JS engine is disabled;
5. Mongo HTTP client & Free Monitoring are disabled (no **libcurl** in KOS CE);

## How to run tests

WIP...

## Changed third-party components

Following list of built-in third-party subprojects which were changed with
respect to enabling KasperskyOS support:

- *src/third_party/asio-master*;
- *src/third_party/boost*;
- *src/third_party/s2*;
- *src/third_party/wiredtiger*;

## Contributing

Please see the [Contributing](./CONTRIBUTING.rst) page for generic info.

We'll follow the parent project contributing rules but would consider to accept
only KasperskyOS-specific changes, so for that it is advised to use
pull-requests.

## Licensing

See [LICENSE-Community.txt](./LICENSE-Community.txt) for the full license text.

[1]: https://github.com/mongodb/mongo
[2]: https://support.kaspersky.com/help/KCE/1.1/en-US/community_edition.htm
[3]: https://www.mongodb.com/
[4]: https://github.com/mongodb/mongo/blob/master/README.md
[5]: https://os.kaspersky.com/development/download/
[6]: https://support.kaspersky.com/help/KCE/1.1/ru-RU/preparing_sd_card_rpi.htm
