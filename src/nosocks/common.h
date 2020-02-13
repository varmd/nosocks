/* NOSOCKS - Control access to internet for non-static apps on Linux

    Copyright (C) 2020 varmd
    Copyright (C) 2000 Shaun Clowes 

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.                                       */

void set_log_options(int, char *, int);
void show_msg(int level, char *, ...);
unsigned int resolve_ip(char *, int, int);

/* Define if you have the ANSI C header files.  */
#define STDC_HEADERS 1

/* Allow tsocks to generate messages to stderr when errors are
encountered, this is really important and should only be disabled if
you're REALLY sure. It can also be turned off at run time, see the man
page for details */



/* dlopen() the old libc to get connect() instead of RTLD_NEXT, 
hopefully shouldn't be needed */
/* #undef USE_OLD_DLSYM */

/* path to library containing connect(), needed if USE_OLD_DLSYM is enabled */
/* #undef LIBCONNECT */

/* path to libc, needed if USE_OLD_DLSYM is enabled */
/* #undef LIBC */

/* Configure the system resolver to use TCP queries on startup, this
allows socksified DNS */
/* #undef USE_SOCKS_DNS */

/* Prototype and function header for connect function */
#define CONNECT_SIGNATURE int __fd, const struct sockaddr * __addr, socklen_t __len

/* The type of socket structure pointer to use to call the 
 * real connect */
#define CONNECT_SOCKARG struct sockaddr *

/* Prototype and function header for select function */
//#define SELECT_SIGNATURE int n, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout


/* Prototype and function header for close function */
//#define CLOSE_SIGNATURE int fd

#undef SOCKET_SIGNATURE

#define SOCKET_SIGNATURE int domain, int type, int protocol

/* Work out which function we have for conversion from string IPs to 
numerical ones */
/* #undef HAVE_INET_ADDR */
#define HAVE_INET_ATON 1

/* Allow the use of DNS names in the socks configuration file for socks
servers. This doesn't work if socksified DNS is enabled for obvious
reasons, it also introduces overhead, but people seem to want it */
#define HOSTNAMES 1

/* We need the gethostbyname() function to do dns lookups in tsocks or 
in inspectsocks */
#define HAVE_GETHOSTBYNAME 1


#define MSGNONE   -1
#define MSGERR    0
#define MSGWARN   1
#define MSGNOTICE 2
#define MSGDEBUG  2
