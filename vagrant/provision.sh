#!/bin/bash

trap 'exit 1' ERR

git clone https://github.com/OpenNetworkingFoundation/libfluid.git
cd libfluid
./bootstrap.sh

cd libfluid_base
./configure --prefix=/usr
make
sudo make install

cd ..
cd libfluid_msg
./configure --prefix=/usr
make
sudo make install

cd /hvbench

rm bin/ -rf
mkdir bin
cd bin/
cmake ../
make

sudo ln -s /vagrant/bin/bin/hvbench /usr/bin/hvbench

# Copy the libfluid to hvbench/bin/bin for docker build
cd /vagrant
cp /usr/lib/libfluid_msg.so.0.0.0 bin/bin/
cp /usr/lib/libfluid_base.so.0.0.0 bin/bin/

echo "Build & installation complete!"

