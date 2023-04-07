# Build the image in 2 steps:
# 1) Debian + KOS CE SDK;
# 2) + MongoDB build environment requirements

ARG DEBIAN_VERSION=10.12
FROM debian:${DEBIAN_VERSION} AS kos-ce-base

WORKDIR /root
ENTRYPOINT bash

RUN apt update
RUN apt upgrade -y
RUN apt install -y wget

# Note: Env variables must be in sync with edition/version of SDK .deb package;
ENV SDK_EDITION=Community-Edition
ENV SDK_VERSION=1.1.1.13
ENV SDK_PATH=/opt/KasperskyOS-${SDK_EDITION}-${SDK_VERSION}

RUN wget https://products.s.kaspersky-labs.com/special/KasperskyOSCommunityEdition/1.1.1.13/multilanguage-1.1.1.13/3637303234377c44454c7c31/KasperskyOS-Community-Edition_1.1.1.13_en.deb
RUN apt install -y ./KasperskyOS-Community-Edition_1.1.1.13_en.deb
RUN rm -rf KasperskyOS-Community-Edition_1.1.1.13_en.deb


# Prepare MongoDB build environment
FROM kos-ce-base AS kos-ce-mongo

# MongoDB build prerequisites:
# https://github.com/mongodb/mongo/blob/master/docs/building.md
RUN apt install -y build-essential \
                   libcurl4-openssl-dev \
                   liblzma-dev \
                   python3 \
                   python3-pip \
                   python3.7-dev \
                   libssl-dev

# Set default python to 3.7.1
RUN update-alternatives --install /usr/bin/python python /usr/bin/python3.7 1

# Update pip for correct cryptography download
RUN python3 -m pip install -U pip

# build_metrics dependencies
RUN python3 -m pip install jsonschema \
                           memory_profiler \
                           puremagic \
                           networkx \
                           cxxfilt \
                           requirements_parser

# We don't clone our mongo repository here for the following reasons:
#
# 1) This is a private repo. Therefore, cloning this repository requires an SSH
# key or HTTPS credentials. Copying this data to an image is not secure.
# 2) For reasons of development. Working with code as usual is easier outside
# of the container. Use volumes (-v) to access code from the container.
#
# But we still need some files from the repository while preparing the mongoDB
# build environment (which happens during the docker image build step). These
# are the Python pip "requirements" files.
# So you need to have <mongo_repo_root>/etc in your docker build context.
#
# TODO: consider different options for placing the repository outside or inside
# (when it becomes public) the container. Pros & cons.
COPY pip ./pip
RUN python3 -m pip install -r pip/compile-requirements.txt && rm -rf ./pip

