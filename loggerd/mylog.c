/*
 * log.c	Logging module.
 *
 * Version:	$Id: log.c,v 1.38 2003/04/01 19:12:42 aland Exp $
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Copyright 2001  The FreeRADIUS server project
 * Copyright 2000  Miquel van Smoorenburg <miquels@cistron.nl>
 * Copyright 2000  Alan DeKok <aland@ox.org>
 * Copyright 2001  Chad Miller <cmiller@surfsouth.com>
 */

#include <stdarg.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

//static const char rcsid[] = "$Id: log.c,v 1.38 2003/04/01 19:12:42 aland Exp $";

#include "mylog.h"

#if HAVE_SYSLOG_H
#	include <syslog.h>
#endif

 /*
 * Logging facility names
 */
static LRAD_NAME_NUMBER levels[] = {
	{ ": Debug: ",          L_DBG   },
	{ ": Info: ",           L_INFO  },
	{ ": Error: ",          L_ERR   },
	{ ": Warning: ",        L_WRN   },
	{ NULL, 0 }
};

static int debug_flag;

#ifndef HAVE_CTIME_R
/*
 *	We use ctime_r() by default in the server.
 *
 *	For systems which do NOT have ctime_r(), we make the
 *	assumption that ctime() is re-entrant, and returns a
 *	per-thread data structure.
 *
 *	Even if ctime is NOT re-entrant, this function will
 *	lower the possibility of race conditions.
 */
/*static char *ctime_r(const time_t *l_clock, char *l_buf, int len)
{
	ctime_s(l_buf, len, l_clock);

	return l_buf;
}*/
#endif

#ifndef HAVE_STRNCASECMP
/*static int strncasecmp(char *s1, char *s2, int n)
{
	int		dif;
	unsigned char	*p1, *p2;
	int		c1, c2;

	p1 = (unsigned char *)s1;
	p2 = (unsigned char *)s2;
	dif = 0;

	while (n != 0) {
		if (*p1 == 0 && *p2 == 0)
			break;
		c1 = *p1;
		c2 = *p2;

		if (islower(c1))
			c1 = toupper(c1);
		if (islower(c2))
			c2 = toupper(c2);

		if ((dif = c1 - c2) != 0)
			break;
		p1++;
		p2++;
		n--;
	}
	return dif;
}*/
#endif

#ifndef HAVE_STRCASECMP

#ifndef uint32
typedef unsigned int uint32;
#endif//uint32
#endif

/*
 *	Convert an integer to a string.
 */
static const char *lrad_int2str(const LRAD_NAME_NUMBER *table, int number,
			 const char *def)
{
	const LRAD_NAME_NUMBER *lnn;

	for (lnn = table; lnn->name != NULL; lnn++) {
		if (lnn->number == number) {
			return lnn->name;
		}
	}

	return def;
}

/*
 *	Log the message to the logfile. Include the severity and
 *	a time stamp.
 */
static int vradlog(FILE * msgfd, int lvl, const char *fmt, va_list ap)
{
	unsigned char *p;
	char buffer[8192];
	uint32 len;

	/*
	 *	NOT debugging, and trying to log debug messages.
	 *
	 *	Throw the message away.
	 */
	if (!debug_flag && (lvl == L_DBG)) {
		return 0;
	}

	if (NULL == msgfd) {
		if (debug_flag) {
	        	msgfd = stdout;
	        }else{
	        	msgfd = stderr;
		}
	}

	{
		const char *s;
		time_t timeval;

		timeval = time(NULL);
		ctime_r(&timeval, buffer);

		s = lrad_int2str(levels, (lvl & ~L_CONS), ": ");

		strcat(buffer, s);
		len = strlen(buffer);
	}

#ifdef HAVE_VSNPRINTF
	vsnprintf(buffer + len, sizeof(buffer) - len -1, fmt, ap);
#else
	vsprintf(buffer + len, fmt, ap);
	if (strlen(buffer) >= sizeof(buffer) - 1)
		/* What can we do? */
		_exit(42);
#endif

	/*
	 *	Filter out characters not in Latin-1.
	 */
	for (p = (unsigned char *)buffer; *p != '\0'; p++) {
		if (*p == '\r' || *p == '\n')
			*p = ' ';
		else if (*p < 32 || (*p >= 128 && *p <= 160))
			*p = '?';
	}
	strcat(buffer, "\n");
	
	/*
	 *   If we're debugging, for small values of debug, then
	 *   we don't do timestamps.
	 */
	p = (unsigned char *)buffer;

	{
		fputs((char*)p, msgfd);
		if (msgfd == stdout) {
			fflush(stdout);
		} else if (msgfd == stderr) {
			fflush(stderr);
		} else {
			fflush(msgfd);
		}
	}

	return 0;
}


int mylog(const char * filename, int lvl, const char *msg, ...)
{
	va_list ap;
	int r;

	FILE * fp;
	if ((fp=fopen(filename, "a")) == NULL)
		return -1;
	va_start(ap, msg);
	r = vradlog(fp, lvl, msg, ap);
	va_end(ap);
	fclose(fp);

	return r;
}

void set_debug_flag(int flag)
{
	debug_flag = flag;
}

int mylog3(FILE * fp, int lvl, const char *msg, ...)
{
	va_list ap;
	int r;

	va_start(ap, msg);
	r = vradlog(fp, lvl, msg, ap);
	va_end(ap);

	return r;
}
