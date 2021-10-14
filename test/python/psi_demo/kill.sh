#!/bin/bash

ps -ef | grep -E "psi" | grep -v grep | awk '{print $2}' | xargs kill -9 >/dev/nul 2>&1
