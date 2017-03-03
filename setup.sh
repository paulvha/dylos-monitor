#!/bin/bash
# 
# Compile script Dylos monitor on linux
#
# Dylos is registered trademark Dylos Corporation
# 2900 Adams St#C38, Riverside, CA92504 PH:877-351-2730
# 
# Copyright (c) 2017 Paul van Haastrecht <paulvha@hotmail.com>
#
# version 1.0 : initial version.
#

clear		#clear window

cc -o dylosmon dylos_mon.c dylos.c 

echo "Type sudo ./dylosmon -h for help"


