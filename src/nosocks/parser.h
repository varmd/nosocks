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

#ifndef _PARSER_H

#define _PARSER_H	1

/* Structure definitions */

/* Structure representing one server specified in the config */
struct serverent {
	int lineno; /* Line number in conf file this path started on */
	char *address; /* Address/hostname of server */
	int port; /* Port number of server */
	int type; /* Type of server (4/5) */
	char *defuser; /* Default username for this socks server */
	char *defpass; /* Default password for this socks server */
	struct netent *reachnets; /* Linked list of nets from this server */
	struct serverent *next; /* Pointer to next server entry */
};

/* Structure representing a network */
struct netent {
   struct in_addr localip; /* Base IP of the network */
   struct in_addr localnet; /* Mask for the network */
   unsigned long startport; /* Range of ports for the */
   unsigned long endport;   /* network                */
	struct netent *next; /* Pointer to next network entry */
};

/* Structure representing a complete parsed file */
struct parsedfile {
   struct netent *localnets;
   struct serverent defaultserver;
   struct serverent *paths;
};

/* Functions provided by parser module */
int read_config(char *, struct parsedfile *);
int is_local(struct parsedfile *, struct in_addr *);
int pick_server(struct parsedfile *, struct serverent **, struct in_addr *, unsigned int port);
char *strsplit(char *separator, char **text, const char *search);

#endif
