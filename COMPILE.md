# COMPILE

## Build in vagrant (recommended)

First clone the repository:

```bash
git clone https://github.com/csieber/hvbench
```

Spin up the virtual machine (this might take a while):

```bash
cd hvbench/vagrant
vagrant up
```

SSH into the virtual machine:

```bash
vagrant ssh
```

Build hvbench from source:

```bash
cd /hvbench
mkdir -p bin
cd bin
cmake ../
make
```

**Hint**: You add the *-DCMAKE_BUILD_TYPE=Debug* to cmake to create a debug build.

The finished build will be in hvbench/bin/bin on the host PC and in /hvbench/bin/bin on the virtual machine.

To build the docker image, execute docker build (-t(ag) is just an example):

```bash
cd /hvbench
sudo docker build -t csieber/hvbench:latest .
```

If you get the error **lstat bin/bin/libfluid_msg.so.0.0.0: no such file or directory** while building the docker image, you have to copy the libfluid_(base/msg) libraries to the bin folder:

```bash
cp /usr/lib/libfluid_base.so.0.0.0 /vagrant/bin/bin/libfluid_base.so.0.0.0
cp /usr/lib/libfluid_msg.so.0.0.0 /vagrant/bin/bin/libfluid_msg.so.0.0.0
```

To push it to a registry, log in and push the image.

```bash
sudo docker login https://index.docker.io/v1/
sudo docker push csieber/hvbench:latest
```

## Build manually

### Dependencies:

 - curl
 - pthread
 - event\_pthreads
 - boost chrono 
 - boost system
 - boost thread
 - boost\_filesystem
 - fluid\_msg
 - fluid\_base

### Compile

```bash
mkdir bin
cd bin/
cmake ../ # or cmake -DCMAKE_BUILD_TYPE=Debug ../
make
```

# REFERENCES
