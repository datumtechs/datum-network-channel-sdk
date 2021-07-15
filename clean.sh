#!/bin/bash

curdir=$(pwd)

find . -name "__pycache__" -type d | xargs rm -rf

rm -rf .eggs
rm -rf build

rm -rf python/channel_sdk.egg-info
rm -rf python/channel_sdk/*.so*

rm python/channel_sdk.egg-info -rf
rm dist -rf

if [ "$USER" == "root" ]; then
  sudo pip3 uninstall channel-sdk -y
else
  python3 -m pip uninstall channel-sdk -y # for the current user
fi
