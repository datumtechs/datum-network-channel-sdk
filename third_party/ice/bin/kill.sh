#!/bin/bash

killall glacier2router icegridregistry >/dev/null 2>&1

ps -ef | grep -E "glacier2router|icegridregistry" | grep -v grep | awk '{print $2}' | xargs kill -9 >/dev/null 2>&1
