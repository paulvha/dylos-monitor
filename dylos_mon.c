/* Dylosmon is monitor to read Dylos devices DC11xx, DC1700
 *  
 * Dylos is registered trademark Dylos Corporation
 * 2900 Adams St#C38, Riverside, CA92504 PH:877-351-2730
 *
 * This file is part of Dylos monitor on linux
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
 * along with Dylos monitor.  If not, see <http://www.gnu.org/licenses/>. 
 * 
 * to compile :
 *  
 * cc -o dylosmon dylos_mon.c dylos.c 
 * 
 */

#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <getopt.h>
#include <sys/stat.h>
#include "dylos_mon.h"

// different device requested
char	*device = NULL;

// stored_data dump requested
int		stored_data = 0;		// request flag for stored data
int		found_dc17 = 0;			// indicate this is a DC17xx

// log file information
char	*logfile = NULL;
int		banner = 1;
FILE	*fp_log = NULL;
int		timestamp = 0;
int		no_check_log = 1;

/* close out correctly 
 * @param ret : exit code */
void close_out(int ret)
{
	char	buf[BUFLEN];
	int		res;
	
	dbprintf(0,"close out.\n");
	
	/* case we were in getting stored data and not finished receiving
	 * (due to signal) we read the into dummy and flush
	 */
	if (stored_data)
	{
		dbprintf(1,"\nWait for Dylos to complete sending stored data request.\n "); 
		do
		{
			res = read_dylos(buf, BUFLEN, 5);
			
			// show progress
			dbprintf(1, ".");
			fflush(stderr);
			
		} while (res > 0);
		
		dbprintf(1,"\ndone\n");
	}
	
	// if logfile in use
	if (fp_log != NULL)	
	{
		dbprintf(0,"closing log file.\n");
		fclose(fp_log);
	}
	
	// free allocated memory
	if (device != NULL)	free(device);
	if (logfile != NULL) free (logfile);
	
	// close connection with Dylos
	close_dylos();
	
	exit(ret);
}

/* catch signals to close out correctly */
void signal_handler(int sig_num)
{
	dbprintf(0,"\nStopping Dylos monitor.\n");
	close_out(0);
}

/* setup catching signals */
void set_signals()
{
	struct sigaction act;

	memset(&act, 0x0, sizeof(act));
	
	act.sa_handler = &signal_handler;
	sigemptyset(&act.sa_mask);
	
	sigaction(SIGTERM,&act, NULL);
	sigaction(SIGINT,&act, NULL);
	sigaction(SIGABRT,&act, NULL);
	sigaction(SIGSEGV,&act, NULL);
	sigaction(SIGKILL,&act, NULL);
}

/* check whether file/device exists
 * @param name : name of file or device
 * will return (0) if exists */
int	check_file(char * name)
{
	struct	stat buffer;
	
	return(stat(name, &buffer));
}

/* check and set log file
 * @param name : name of logfile to set  */
void set_log_file(char *name)
{
	int c,res;
	
	// check that real logfile name was provided (and not a next option)
	if(name[0] == '-')
	{
		dbprintf(1,"Log filename '%s'should not start with '-'\n",name);
		exit(0);
	}	
	
	// should we check for logfile existence
	if (no_check_log) res = 1; 
	else res = check_file(name);
	 
	// if logfile exists already
	if (res == 0) 
	{
		printf("Log file: '%s' exists. Continue to overwrite ? (y or n) ", name); 
	
		//ask question
		c = getchar();
		
		if (c == 'n' ||  c == 'N') exit(0);
	}
	else
		dbprintf(0,"log file '%s'will be used.\n",name);
	
	// set logfile	
	logfile=strdup(name);
}

/* check that requested device name exists and set device
 * @param name : name of device to set
 */
void set_device(char * name)
{
	// check whether device exists
	if (check_file(name) == 0)	device = strdup(name);
	else {
		dbprintf(1,"Device %s does not exist !\n", name);
		exit(-1);
	}
}

/* display usage information 
 * @param name : name of current program */
void usage(char	*name)
{
	printf("usage: sudo %s  [-l logfile] [-D device] [-h] [-b] [-m] [-v] [-d]\n"
	
		"\nCopyright (c)  2017 Paul van Haastrecht.\n"
		"\n"
		"-l, 	sets logfile and enable storage.\n"
		"\t-b, 	do not add banners in logfile.\n"
		"\t-t, 	add timestamp (dd-mm-yy) in logfile.\n"
		"\t-T, 	add timestamp (mm-dd-yy) in logfile.\n"
		"\t-c	no check on logfile existence. (place before -l)\n"
		"-D, 	use different device than default: %s\n"
		"-s, 	read stored data in memory from Dylos DC1700.\n"
		"-h, 	display help information.\n"
		"-d, 	enable debug progress information.\n"
		"-v,	display version information.\n"	,name, DYLOS_USB);
}

int main (int argc, char **argv)
{
	int 	c;
	
	// parse arguments
	while (1)
	{
		c = getopt(argc, argv,"-l:D:bchvsdtT");

		if (c == -1)	break;
			
		switch (c) {
			case 'l':	// sets and enable logfile
				set_log_file(optarg);
				break;
			
			case 'D':	// sets new device port
				set_device(optarg);
				break;
			
			case 'b':	// no banners
				banner = 0;
				break;

			case 'c':	// no check on logfile existence
				no_check_log = 1;
				break;
								
			case 'd':	// enable debug progress messages
				debug_dylos(1);
				break;
				
			case 's':	// get history stored_data dump
				stored_data = 1;
				break;
	
			case 't':	// add /use timestamp (dd-mm-yy)
				timestamp = 1;
				break;

			case 'T':	// add /use timestamp (mm-dd-yy)
				timestamp = 2;
				break;										
			
			case 'h':	// help
				usage(argv[0]);
				exit(0);
				break;
				
			case 'v':	//version 
				printf("Version %s\n", MON_VERSION);
				exit(0);
				break;
				
			default:
				dbprintf(1,"Incorrect argument. Try %s -h\n", argv[0]);
				exit(-1);
				break;
			}
	}

	// set to catch signals
	set_signals();
		
	// open connection to Dylos
	if (open_dylos(device) != 0) exit(-1);
	
	// read Dylos unit information
	if (get_dylos_info() < 0)	close_out(-1);
	
	// get Dylos data
	if (get_dylos_data() < 0)	close_out(-1);
	
	// close out correctly
	close_out(0);
	
	// to stop -Wall complain
	exit(0);
}	


// get Dylos unit information
int get_dylos_info()
{
	char	buf[BUFLEN];		// hold Dylos input
	char	t_buf[BUFLEN];		// hold raw output to display/log
	int		loop = 2;			// wait max 2 x 5 = 10 seconds
	int		ret, i, offset = 0;
	
	// get Dylos name
	if (ask_device_name() != 0)
	{
		dbprintf(1,"Error during asking device unit information.\n");
		close_out(-1);
	}
	
	dbprintf(0,"Getting Dylos unit information.\n");
	
	// read complete device name
	while(loop > 0)
	{
		// try to read from Dylos and wait 5 seconds
		ret = read_dylos(buf, BUFLEN, 5);

		// if data received
		if (ret > 0)
		{
			for(i = 0; i < ret; i++)
			{
				// if last byte on line
				if (buf[i] == 0xa )
				{
					// terminate 
					t_buf[offset] = 0x0;
					
					// to screen
					printf("Device information : %s\n",t_buf);
					
					// try to log
					log_dylos_info(t_buf);
					
					// now look for 17xx (as it allows stored data)
					if(strstr(t_buf, "DC17") != NULL)
						found_dc17 = 1;
					
					// break loop
					loop = -1;
				}
				
				// skip carriage return and any carbage below 'space'
				else if (buf[i] != 0xd && buf[i] > 0x1f) 
	
					t_buf[offset++] = buf[i];
			}
		}
		else
			loop--;
	}
		
	if (loop == 0) {
		dbprintf(1,"time-out : can not read device name.\n");
		return(-1);
	}
	
	return(0);
}

// get Dylos data and display / log
int get_dylos_data()
{
	char	buf[BUFLEN];		// holds data as read from Dylos
	char	t_buf[BUFLEN];		// holds raw-data to display/log
	int		loop = 2;			// wait max 2 x 3 = 6 seconds for next data 
	int		ret, i, offset=0;
	
	dbprintf(0,"Waiting for Dylos data.\n");
	
	// if requested get Dylos log data
	if (stored_data)
	{
		// check this a device with stored data
		if ( ! found_dc17)
		{
			dbprintf(1,"can not provide stored data as device is not DC17xx.\n");
			stored_data = 0;		// reset request (needed in close_out() )
			return(-1);
		}
			
		if (ask_log_data() != 0)
		{
			dbprintf(1,"Error during asking stored data.\n");
			close_out(-1);
		}
	}
	
	// read next entry
	while(loop > 0)
	{
		// try to read from Dylos and wait 3 seconds
		ret = read_dylos(buf, BUFLEN, 3);

		// if data received
		if (ret > 0)
		{
			for(i = 0; i < ret; i++)
			{
				// if last byte on line
				if (buf[i] == 0xa) 
				{
					// terminate
					t_buf[offset] = 0x0;
					
					// to screen
					printf("%s\n",t_buf);
					
					// try to log in case of 
					if (log_dylos_data(t_buf, offset) < 0) {
						dbprintf(1, "Error during saving log file '%s'\n",logfile);
						close_out(-1);
					}
					
					// reset offset
					offset=0;
				}
				// skip carriage return and any carbage below 'space'
				else if (buf[i] != 0xd && buf[i] > 0x1f) 

					t_buf[offset++] = buf[i];
			}
		}
		// if error
		else if (ret < 0)	return(ret);
			
		else     // time_out counter during stored data
			if (stored_data) loop--;	
	}
		
	dbprintf(0,"Ready with reading stored data from DC1700.\n");
	
	// indicate that we are done with receiving stored data
	stored_data = 0;
	
	return(0);
}

/*Format device information for output to log
 * @param buf: information data to log  */
int log_dylos_info(char *buf)
{
	char	outbuf[40];
	char	tbuf[BUFLEN];
	
	time_t 	r_time;		// get current time
	struct	tm	*timeinfo;
	
	// if not logging nor banners requested return
	if (logfile == NULL || banner == 0)	return(0);	

	log_dylos("------------------------------");
	
	sprintf(outbuf, "Dylos Linux Logger v %s", MON_VERSION);
	log_dylos(outbuf);
	
	sprintf(outbuf, "Unit: %s", buf);
	log_dylos(outbuf);
	
	// add time and date stamp
	time (&r_time);
	timeinfo=localtime(&r_time);
	
	
	if(timestamp == 1) // if -t
	{
		strftime(tbuf,sizeof(tbuf), "%e-%m-%y %R",timeinfo);
		sprintf(outbuf, "dd-mm-yy/Time : %s",tbuf);
	}
	else      // -T
 	{
		strftime(tbuf,sizeof(tbuf), "%m-%e-%y %R",timeinfo);
		sprintf(outbuf, "mm-dd-yy/Time : %s",tbuf);
	}
	
	return(log_dylos(outbuf)); 
		
}

/* Format data for logging in case of monitor mode
 * @param buf: line of measured raw data to log
 * @param len: length of line  */
int log_dylos_monitor(char *buf)
{
	static	int	header = 1;
	char	outbuf[100], tbuf[40];
	time_t 	r_time;		// get current time
	struct	tm	*timeinfo;

	// clear out buffer
	memset(outbuf,0x0,sizeof(buf));
	
	// set header if allowed (once)
	if (header)
	{
		// prevent header
		if (banner == 1)
		{
			log_dylos("------------------------------");
			log_dylos("Particles per cubic foot / 100");
			log_dylos("------------------------------");
			
			if (timestamp == 1)
				log_dylos("dd-mm-yy, Time, Small, Large");
			else if (timestamp == 2)
				log_dylos("mm-dd-yy, Time, Small, Large");
			else
				log_dylos("Small,Large");
		}
		
		header = 0;
	}

	// add time and date stamp
	if (timestamp)
	{
		time (&r_time);
		timeinfo=localtime(&r_time);
		
		if (timestamp = 1) // dd-mm-yy  -t
			strftime(tbuf,sizeof(tbuf), "%e-%m-%y, %R",timeinfo);
			
		else  // mm-dd-yy  -T
			strftime(tbuf,sizeof(tbuf), "%m-%e-%y, %R",timeinfo);
		
		sprintf(outbuf, "%s, %s",tbuf, buf);
	}
	else
		sprintf(outbuf, "%s",buf);
	
	return(log_dylos(outbuf)); 
}

/* Format output to log in case of data from DC1700
 * @param buf : line of measured raw data to log
 * @param len : length of line  */
int log_dylos_data(char *buf, int len)
{
	
	// if no logging requested
	if (logfile == NULL)	return(0);
	
	// in case reading stored data
	if (stored_data) return(log_dylos_stored(buf,len));
	else return(log_dylos_monitor(buf));
}

/* Format output to log in case of dump stored data DC1700
 * @param buf : line of measured raw data to log
 * @param len : length of line  */
int log_dylos_stored(char *buf, int len)
{
	char	*outbuf;
	int 	i,j;
	static	int header = 1;		// remember header was done
	char	mon[3], day[3];		// needed for -t option
	
	/* A received line less than 15 char (length date and timestamp), 
	 * is not coming from stored but an actual monitor reading 
	 * that is added. As such it is excluded from stored values  */
	if (len < 15) return(0);

	// allocate memory for output buffer (5 to allow for adding spaces)
	outbuf = malloc(len + 5);

	if (outbuf == NULL){
		dbprintf(1,"can not allocate memory to outpur stored data.\n");
		close_out(-1);
	}
	
	if (header)
	{
		// if requested prevent header
		if (banner == 1)
		{
			log_dylos("------------------------------");
			log_dylos("Particles per cubic foot / 100");
			log_dylos("------------------------------");

			if(timestamp == 1) 	// if -t
				log_dylos("dd-mm-yy Time Small, Large");
			else      			// -T (standard from Dylos)
				log_dylos("mm-dd-yy Time Small, Large");
		}
		
		header = 0;
	}
	
	/* Stored timestamp default mm-dd-yy. (like -T)
	 * if dd-mm-yy  (-t) is requested, swap month and day
	 */
	
	if (timestamp == 1)
	{
		strncpy(mon,buf,2);		// get month
		strncpy(day,buf+3,2);	// get day
		day[2]=mon[2]=0x0;		// terminate string
		
		sprintf(outbuf,"%s-%s-%s",day,mon,buf+6);
		strcpy(buf,outbuf);
	} 
	// format the data
	for (i = 0, j = 0; i < len; i++)
	{
		outbuf[j++] = buf[i];
		
		// change / to -
		if (buf[i] == '/') outbuf[j-1] = '-';
		
		// add space after comma
		if (buf[i] == ',') outbuf[j++] = ' ';
	}
	
	//add terminater
	outbuf[j] = 0x0;
	
	i = log_dylos(outbuf);
	
	// memory
	free(outbuf);
	
	return(i);
}

/* write to log file 
 * @param buf: line of formated data to log */
int log_dylos(char *buf)
{
	// if logfile is not open yet
	if (fp_log == NULL)
	{
		// open file. if existing (checked in set_log_file) overwrite
		fp_log = fopen(logfile,"w");
		
		if (fp_log == NULL)
		{
			dbprintf(1,"can not open logfile '%s'.\n",logfile);
			close_out(-1);
		}
		else
			dbprintf(0,"log file '%s' opened.\n",logfile);

	}

	// write to logfile
	return(fprintf(fp_log, "%s\n", buf));	
}
