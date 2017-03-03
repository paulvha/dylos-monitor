Dylosmon is a Dylos logger on a Linux system.

Dylos is (r) registered trademark Dylos Corporation
2900 Adams St#C38, Riverside, CA92504 PH:877-351-2730

Windows is a (r) registered trademark of Microsoft Corporation.

---------------------------------------------------------------------

# version 1.0	initial version

Copyright (c) 2017 Paul van Haastrecht <paulvha@hotmail.com>

This dylosmon utility will read, display and log the information and 
data from a DYLOS device on Linux.

The output is logged in same format as the Windows Dylos_logger 
software as provided by the Dylos Corporation. The big difference is 
that it can run in the background without human intervention and has
optional parameters for formatting the logged data. 

It has been tested on a Raspberry Pi-3 (Jessie) and Ubuntu 16.04 
with a DC1700.

Dylosmon is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by the
Free Software Foundation, either version 3 of the License, or (at your
option) any later version.
----------------------------------------------------------------------

## Description

The following are available options for dylosmon:

No option : read and display the unit information followed, every 
minute, with the measured values as provided by the Dylos device.

-D device : set different device than default "/dev/ttyUSB0" for Dylos.

-l logfile : will set and enable (next to display) logging in 
the file in the same format as the Windows DYLOS-logger software. This
can be used in combination with following options

	-b : exclude text headers and banners from the logfile.

	-t : add / use timestamp (dd-mm-yy) in logfile.
	
	-T : add / use timestamp (mm-dd-yy) in logfile.
	
	-c : do not check for logfile existence. (MUST to be placed
	     BEFORE -l option)

-s : display (log with -l option) the stored values/history from DC1700.

-d : enable debug progress messages.

-v : show the version number.

-h : display help text.


## compile

Run command `./setup.sh` in the install directory to compile.

## usage

Make sure to connect the Dylos to a USB port on the Linux system.

Given the default permissions on /dev/ttyUSBx, the utility has to be 
run as superuser/root. To start  `sudo ./dylosmon` [options]

In case this is run at start-up or from cron, this permission will not 
be an issue as both are run as superuser/root.

see example_usage.txt for output examples.

## to-do
- integrate with on-line and/or off-line graphical display.
- create possibility to store the logdata into an access database and
  or spreadsheet.
