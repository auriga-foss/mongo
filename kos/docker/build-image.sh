#!/bin/bash

SCRIPT_DIR="$(dirname "$(realpath "${0}")")"

docker build -t debian-kos-ce-mongo:1.1.1.13 -f Dockerfile ${SCRIPT_DIR}/../../etc/ \
    && echo "Docker image build status: SUCCESS" \
    || echo "Docker image build status: FAILED"
