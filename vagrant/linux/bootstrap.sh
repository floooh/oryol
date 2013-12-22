#!/usr/bin/env bash

# install required packaged (TODO this should go through chef in the future)
apt-get update
apt-get install -y python-software-properties
add-apt-repository ppa:ubuntu-toolchain-r/test
apt-get update
apt-get install -y vim
apt-get install -y cmake
apt-get install -y gcc-4.8
apt-get install -y g++-4.8
apt-get install -y make
apt-get install -y libcurl4-gnutls-dev
apt-get install -y git

update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-4.8 50
update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-4.8 50
update-alternatives --install /usr/bin/c++ c++ /usr/bin/g++-4.8 50

