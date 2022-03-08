#!/bin/bash

ps -ef | grep -E "glacier2|icegrid" | grep -v grep | awk '{print $2}'