#!/bin/bash

SCRIPT_DIR="$(dirname "$(realpath "${0}")")"

# Note: Instead of default "bridge" network, you can create your own docker network and
# share it between containers.
docker run -it --name ${USER}-kos-ce-mongo \
    -v ${SCRIPT_DIR}/../../:/root/mongo \
    --hostname kos-mongod-server \
    "$@" \
    debian-kos-ce-mongo:1.1.1.13
