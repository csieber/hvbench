#!/bin/bash

trap 'exit 1' ERR

# Add docker GPG key and apt repository
apt-key adv --keyserver hkp://p80.pool.sks-keyservers.net:80 --recv-keys 58118E89F3A912897C070ADBF76221572C52609D
echo "deb https://apt.dockerproject.org/repo ubuntu-trusty main" > /etc/apt/sources.list.d/docker.list

apt-get update
apt-get update

# Install dependencies
apt-get install -y autoconf libtool build-essential pkg-config
apt-get install -y libevent-dev libssl-dev libtclap-dev
apt-get install -y libboost-thread-dev libboost-filesystem-dev libboost-chrono-dev libboost-system-dev
apt-get install -y libcurl4-openssl-dev
apt-get install -y git cmake

# Install docker
apt-get install -y linux-image-extra-$(uname -r)
apt-get install -y docker-engine

echo "Installation (root) complete!"

