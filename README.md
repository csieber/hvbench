# hvbench: An open and scalable SDN hypervisor benchmark

hvbench is an open and scalable OpenFlow controller/switch/hyperisor benchmark based on message type weights. A central API controls distributed message generators (acting as switches or controllers) and run-time statistics such as delay are fed back to the API or external programs.

Following projects are related to this project:

  - [hvbench-api](https://github.com/csieber/hvbench-api): The Python API of hvbench
  - [hvmonitor](https://github.com/csieber/hvmonitor): Monitor the resource comsumption of a network hypervisor
  - [tutorial](https://github.com/csieber/hvbench-tutorial): Tutorial on how to set-up and operate hvbench

## Features
  
  * Distributed operation for scalability
  * Centrally controlled & configured
  * Add, reconfigure  and remove tenants at run-time
  * Add, connect, remove switches at run-time (work in progress)
  * Static or exponential (Poisson) inter-arrival times
  * One message type or mix of message types based on weights
  * Support for OpenFlow 1.0 (1.3 in working)

## QUICKSTART

See [QUICKSTART.md](QUICKSTART.md).

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
