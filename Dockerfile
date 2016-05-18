FROM ubuntu:14.04
MAINTAINER Christian Sieber <c.sieber@tum.de>

RUN apt-get update && \
    apt-get install -y libboost-system1.54.0 libboost-thread1.54.0 libboost-filesystem1.54.0 libboost-chrono1.54.0 libcurl3 libevent-2.0-5 libevent-openssl-2.0-5 libevent-pthreads-2.0-5 && \
    rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/*

# hvbench
COPY bin/bin/hvbench /opt/hvbench/

# libfluid
COPY bin/bin/libfluid_msg.so.0.0.0 /usr/lib/libfluid_msg.so.0
COPY bin/bin/libfluid_base.so.0.0.0 /usr/lib/libfluid_base.so.0

# OpenFlow Tenant Ports
EXPOSE 6633-6733

WORKDIR /opt/hvbench
ENTRYPOINT ["./hvbench"]
