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
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA. */

#ifndef _NOSOCKS_H

#define _NOSOCKS_H	1

#include <parser.h>

/* Structure representing a socks connection request */
struct sockreq {
   int8_t version;
   int8_t command;
   int16_t dstport;
   int32_t dstip;
   /* A null terminated username goes here */
};

/* Structure representing a socks connection request response */
struct sockrep {
   int8_t version;
   int8_t result;
   int16_t ignore1;
   int32_t ignore2;
};

/* Structure representing a socket which we are currently proxying */
struct connreq {
   /* Information about the socket and target */
   int sockid;
   struct sockaddr_in connaddr;
   struct sockaddr_in serveraddr;

   /* Pointer to the config entry for the socks server */
   struct serverent *path;

   /* Current state of this proxied socket */
   int state;

   /* Next state to go to when the send or receive is finished */
   int nextstate;

   /* When connections fail but an error number cannot be reported 
    * because the socket is non blocking we keep the connreq struct until
    * the status is queried with connect() again, we then return
    * this value */
   int err;

   /* Events that were set for this socket upon call to select() or
    * poll() */
   int selectevents;

   /* Buffer for sending and receiving on the socket */
   int datalen;
   int datadone;
   char buffer[1024];

   struct connreq *next;
};

/* Connection statuses */
#define UNSTARTED 0
#define CONNECTING 1
#define CONNECTED 2
#define SENDING 3
#define RECEIVING 4
#define SENTV4REQ 5
#define GOTV4REQ 6 
#define SENTV5METHOD 7 
#define GOTV5METHOD 8
#define SENTV5AUTH 9
#define GOTV5AUTH 10
#define SENTV5CONNECT 11
#define GOTV5CONNECT 12
#define DONE 13 
#define FAILED 14 
   
/* Flags to indicate what events a socket was select()ed for */
#define READ (1<<0)
#define WRITE (1<<1)
#define EXCEPT (1<<2)
#define READWRITE (READ|WRITE)
#define READWRITEEXCEPT (READ|WRITE|EXCEPT)

#endif
