# Quickstart

We recommend Docker to run hvbench, kafka+zookeeper and etcd.

## Prerequisites

  * Docker: [docker-engine](https://docs.docker.com/engine/installation/linux/ubuntulinux/)
  * Docker-compose: (highly recommended) [docker-compose](https://docs.docker.com/compose/install/)
  
*Docker-compose* allows to start several container with one command, which we recommand to set-up *etcd* and *kafka+zookeeper*.

## Run

1. First open the **docker-compose.yml** file and adapt *KAFKA_ADVERTISED_HOST_NAME* to the public IP of your host. 

    Otherwise kafka will advertise the wrong IP to the clients.


2. Afterwards use *docker-compose* to start *etcd* and *kafka*+*zookeeper*:

    ```bash
    sudo docker-compose up
    ```

3. Then start one or multiple *hvbench* instances as daemon with port exposed on the host PC. 
  
    As IP, either use your external etcd IP (if you connect from a different machine), or the docker internal IP of your host (if you have everything on one machine) as seen on the docker bridge with **sudo ip addr show docker0**. 

    ```bash
    sudo docker run -d -p 6633-6733:6633-6733 csieber/hvbench -e 172.17.0.1
    ```

    In the default Dockerfile, Docker will expose ports 6633 to 6733 from hvbench.

## Usage

```
USAGE: 

   ./bin/hvbench  [-i <int>] [-p <int>] [-k <string>] [-e <string>] [--]
                  [--version] [-h]


Where: 

   -i <int>,  --id <int>
     Instance ID

   -p <int>,  --port <int>
     etcd Port

   -k <string>,  --kafka <string>
     kafka IP

   -e <string>,  --etcd <string>
     etcd IP

   --,  --ignore_rest
     Ignores the rest of the labeled arguments following this flag.

   --version
     Displays version information and exits.

   -h,  --help
     Displays usage information and exits.
```

