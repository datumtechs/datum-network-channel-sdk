#!/bin/bash

curdir=$(pwd)

find . -name "__pycache__" -type d | xargs rm -rf

rm -rf .eggs
rm -rf build

rm -rf python/io_channel.egg-info
rm -rf python/io_channel/*.so*

rm python/io_channel.egg-info -rf
rm dist -rf

if [ "$USER" == "root" ]; then
  sudo pip3 uninstall io-channel -y
else
  python3 -m pip uninstall io-channel -y # for the current user
fi
