#!/bin/bash
cd ~
curl -O http://ibsimu.sourceforge.net/libibsimu-1.0.6dev_9cbf04.tar.gz
tar zxvf libibsimu-1.0.6dev_9cbf04.tar.gz
cd libibsimu-1.0.6dev_9cbf04
./configure --prefix=/home/ubuntu
make
make check
make install
echo "export LD_LIBRARY_PATH=/home/ubuntu/lib" >> ~/.bashrc
echo "export PKG_CONFIG_PATH=/home/ubuntu/lib/pkgconfig" >> ~/.bashrc
