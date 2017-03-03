/* Header file for Dylos monitor routines
 * 
 * Dylos is registered trademark Dylos Corporation
 * 2900 Adams St#C38, Riverside, CA92504 PH:877-351-2730
 *
 * This file is part of Dylos monitor.
 *  
 * Copyright (c) 2017 Paul van Haastrecht <paulvha@hotmail.com>
 *
 * Dylosmon is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or (at your
 * option) any later version.
 *
 * Dylosmon is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Dylos monitor.  If not, see <http://www.gnu.org/licenses/>
 */
 
# include "dylos.h"

#define BUFLEN	256				// length of buffers
#define MON_VERSION "1.0"		// current version

/* catch signals to close out correctly */
void signal_handler(int sig_num);

/* setup catching signals */
void set_signals();

/* display usage information 
 * @param name: name of current program */
void usage(char	*name);

/* checks whether file/device exists
 * @param name : name of file or device
 * will return (0) if exists */
int	check_file(char * name);

/* checks and set log file
 * @param name : name of logfile to set */
void set_log_file(char *name);

/* check that requested device name exists and set device
 * @param name : name of device to set */
void set_device(char * name);

/* get Dylos unit information */
int get_dylos_info();

/* get Dylos data and display / log */
int get_dylos_data();

/*format device information for output to log
 * @param buf : information data to log */
int log_dylos_info(char *buf);

/*format data for output to log
 * @param buf : line of measured raw data to log
 * @param len : length of line */
int log_dylos_data(char *buf, int len);

/* format data in case of monitor mode
 * @param buf : line of measured raw data to log
 * @param len : length of line  */
int log_dylos_monitor(char *buf);

/* Format data for output to log in case of dump stored data DC1700
 * @param buf : line of measured raw data to log
 * @param len : length of line  */
int log_dylos_stored(char *buf, int len);

/* write to log file 
 * @param buf : line of formated data to log */
int log_dylos(char *buf);

/* close out correctly 
 * @param ret : exit code */
void close_out(int ret);
