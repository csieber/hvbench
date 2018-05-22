# hvbench: An open and scalable SDN hypervisor benchmark

hvbench is an open and scalable OpenFlow controller/switch/hyperisor benchmark based on distributed message generators. A central API configures the message generators (which act as switches or controllers) and collects run-time statistics such as the response delay. Check out the OSSN paper [here](https://mediatum.ub.tum.de/doc/1415919/file.pdf) for an in-depth overview.

![Overview](/doc/overview.png)

Following repositories are related to this project:

  - [hvbench-api](https://github.com/csieber/hvbench-api): The Python API of hvbench
  - [hvmonitor](https://github.com/csieber/hvmonitor): Monitor the resource comsumption of a network hypervisor
  - [tutorial](https://github.com/csieber/hvbench-tutorial): Tutorial on how to set-up and operate hvbench

## Features
  
  * Distributed operation for scalability
  * Centrally controlled & configured through etcd
  * Add, reconfigure  and remove tenants at run-time
  * Add, connect, remove switches at run-time (work in progress)
  * Static or exponential (Poisson) inter-arrival times
  * One message type or mix of message types based on weights
  * Support for OpenFlow 1.0 (1.3 in working)

## TUTORIALS

See [hvbench-tutorial](https://github.com/csieber/hvbench-tutorial) for tutorials on how to set-up and operate hvbench.

## QUICKSTART

See [QUICKSTART.md](QUICKSTART.md). Setting up and operating hvbench is rather complex, we recommend to follow the tutorials first.

## COMPILE & DOCKER BUILD

See [COMPILE.md](COMPILE.md) for instructions how to compile hvbench and how to build the docker image.

## CITATION

If you are using hvbench for scientific publications, please cite the following paper:

C. Sieber, A. Blenk, A. Basta and W. Kellerer, *hvbench: An open and scalable SDN Network Hypervisor Benchmark*, NetSoft 2016 - Workshop on OSSN

Bibtex:

```
@INPROCEEDINGS{sieber16hvbench,
    AUTHOR="Christian Sieber and Andreas Blenk and Arsany Basta and Wolfgang Kellerer",
    TITLE="hvbench: An open and scalable SDN Network Hypervisor Benchmark",
    BOOKTITLE="NetSoft 2016 - Workshop on OSSN () ",
    MONTH="jun",
    YEAR="2016",
} 
```
