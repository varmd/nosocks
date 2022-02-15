/*

    NOSOCKS - Control access to internet for non-static apps on Linux

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
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/

#define _GNU_SOURCE

//regex
//https://github.com/kokke/tiny-regex-c

/* Global configuration variables */
char *progname = "libnosocks";   /* Name used in err msgs    */

/* Header Files */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dlfcn.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <strings.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/poll.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <pwd.h>
#include <errno.h>
#include <fcntl.h>
#include <common.h>
#include <stdarg.h>
#include <resolv.h>


#include <parser.h>
#include <nosocks.h>

/* Global Declarations */

extern char *program_invocation_short_name;


static int (*realconnect)(CONNECT_SIGNATURE);
//static int (*realselect)(SELECT_SIGNATURE);
//static int (*realpoll)(POLL_SIGNATURE);
//static int (*realclose)(CLOSE_SIGNATURE);
static int (*realsocket)(SOCKET_SIGNATURE);

static struct parsedfile *config;
static struct connreq *requests = NULL;
static int suid = 0;

static char *conffile = NULL;

char global_app_name[255] = "";
char global_app_long_name[255] = "";

char *global_app_full_name = NULL;
static int global_init_done = 0;
static int global_init_allowed = 0;

/* Exported Function Prototypes */
void _init(void);
int connect(CONNECT_SIGNATURE);
int socket(SOCKET_SIGNATURE);

//int select(SELECT_SIGNATURE);
//int poll(POLL_SIGNATURE);
//int close(CLOSE_SIGNATURE);



/* Private Function Prototypes */
static int get_config();
static int get_environment();



void *clean_string(char *string, char *copy) {

  char* space_position = strchr(string, ' ');
  if(space_position) {
     char *token;
     token = strtok(string, " ");
     strcpy(copy, token);

  } else {
    strcpy(copy, string);
  }
}


void _init(void) {


	/* We could do all our initialization here, but to be honest */
	/* most programs that are run won't use our services, so     */
	/* we do our general initialization on first call            */

   /* Determine the logging level */
   suid = (getuid() != geteuid());


	realconnect = dlsym(RTLD_NEXT, "connect");
	realsocket = dlsym(RTLD_NEXT, "socket");

  if(!realsocket) {

    void *dlh;
     dlh = dlopen("libc.so", RTLD_LAZY);

    if (dlh) {
      realsocket = dlsym(dlh, "socket");
    }
  }
  
  get_environment();
  get_config ();
  
  //
  global_app_full_name = (char *)malloc(4096);
  global_app_full_name[0] = NULL;
  int fd = open("/proc/self/cmdline", O_RDONLY);
  
  int size = read(fd, global_app_full_name, 4096);
  close(fd);
  if(size > 0) {
    for(int i = 0 ; i < size; i++){
      //parser.c can't handle whitespace for now use - instead in nosocks.conf
      if(!global_app_full_name[i] && i < (size-1) ) 
        global_app_full_name[i] = '-';
    }
    if(is_app_allowed(global_app_full_name)) {
      global_init_allowed = 1;
    }  
  }
  
  
  

  global_app_long_name[0] = NULL;

  if(program_invocation_short_name) {
    //FILE *logfile = NULL;
    //logfile = fopen("/tmp/lgfile", "w+");
    //fprintf(logfile, " app is %s %s \n", program_invocation_short_name, global_app_name);
    clean_string(program_invocation_short_name, global_app_name);

    //fclose(logfile);
  }
  global_init_done = 1;
}





static int get_environment() {
   static int done = 0;
   int loglevel = MSGERR;
   char *logfile = NULL;
   char *env;

   if (done)
      return(0);

   set_log_options(-1, logfile, 0);


   if ((env = getenv("NOSOCKS_DEBUG"))) {
      loglevel = atoi(env);
      //printf("level is %d \n", loglevel);
   }

   if (((env = getenv("NOSOCKS_DEBUG_FILE"))) && !suid)
      logfile = env;


   set_log_options(loglevel, logfile, 1);



   done = 1;

   return(0);
}

static int get_config () {
   static int done = 0;

   if (done)
      return(0);

   /* Determine the location of the config file */

   conffile = "/etc/nosocks/nosocks.conf";

	/* Read in the config file */
   config = malloc(sizeof(*config));
   if (!config)
      return(0);
	read_config(conffile, config);
   if (config->paths)
      show_msg(MSGDEBUG, "First lineno for first path is %d\n", config->paths->lineno);

   done = 1;

	return(0);

}




int socket(SOCKET_SIGNATURE) {


  if(!global_init_done) { //special case for readelf
    _init();
    global_init_done = 1;
  }
  //always enable for root
  //TODO disable later

  if(getuid() == 0 || global_init_allowed) {
    return realsocket(domain, type, protocol);
  }




  //Return out on local connections early for better performance
  if ((domain == AF_UNIX) || (domain == AF_LOCAL) || (domain == AF_NETLINK) ) {
    return realsocket(domain, type, protocol);
  }


  




  show_msg(MSGDEBUG, "app name is %s for socket() \n", global_app_name );
  show_msg(MSGDEBUG, "app name long %s -- %s \n", global_app_long_name, program_invocation_name);
  show_msg(MSGDEBUG, "app name full %s -- %s \n", global_app_full_name, program_invocation_name);
  





  if(is_app_allowed( global_app_name )) {
     global_init_allowed = 1;
     show_msg(MSGDEBUG, "socket - Global Allowing app %s for socket() \n", global_app_name);
     show_msg(MSGDEBUG, "socket - Long Allowing app %s for socket() \n", program_invocation_name);
     show_msg(MSGDEBUG, "socket - Short Allowing app %s for socket() \n", program_invocation_short_name);

     return realsocket(domain, type, protocol);
   } //



   if(global_app_long_name[0] == NULL)
     strcpy(global_app_long_name, program_invocation_name);
   
  //if(global_app_full_name[0] == NULL)
  //  strcpy(global_app_full_name, "");

   //some wine apps do not fill program_invocation_name on startup
   if( global_app_long_name[0] != NULL && is_app_allowed( global_app_long_name ) ) {
     show_msg(MSGDEBUG, "socket - Global Allowing app %s for socket() \n", global_app_long_name);
     return realsocket(domain, type, protocol);
   }


   show_msg(MSGDEBUG, "Refusing app %s for socket() \n", global_app_name);
   show_msg(MSGDEBUG, "Refusing app %s for socket() \n", global_app_long_name);
   show_msg(MSGDEBUG, "Refusing app %s for socket() \n", global_app_full_name);

  return(-1);


}




int connect(CONNECT_SIGNATURE) {

  //always enable for root
  //TODO add option to disable later
  if(getuid() == 0 || global_init_allowed) {
    return(realconnect(__fd, __addr, __len));
  }

  return(realconnect(__fd, __addr, __len));

  struct sockaddr_in *connaddr;
	struct sockaddr_in peer_address;
	struct sockaddr_in server_address;
  int gotvalidserver = 0, rc, namelen = sizeof(peer_address);
	int sock_type = -1;
	int sock_type_len = sizeof(sock_type);
	unsigned int res = -1;
	struct serverent *path;
  struct connreq *newconn;



  get_environment();


  /* If the real connect doesn't exist, we're stuffed */
  //Disable for performance
  /*
	if (realconnect == NULL) {
		show_msg(MSGERR, "Unresolved symbol: connect\n");
		return(-1);
	}*/

	connaddr = (struct sockaddr_in *) __addr;



  //Return out early for better performance
  if ((connaddr->sin_family == AF_UNIX) || (connaddr->sin_family == AF_LOCAL) || (connaddr->sin_family == AF_NETLINK) ) {
      return(realconnect(__fd, __addr, __len));
      //return(-1);
   }

	/* Get the type of the socket */
	getsockopt(__fd, SOL_SOCKET, SO_TYPE,
		   (void *) &sock_type, &sock_type_len);

	/* If this isn't an INET socket for a TCP stream we can't  */
	/* handle it, refuse           */
   if ((connaddr->sin_family != AF_INET) && (connaddr->sin_family != AF_INET6) ) {
      show_msg(MSGDEBUG, "Refusing non tcp access \n");
      errno = ECONNREFUSED;
      return(-1);
   }


   /* If the address is local call realconnect */
   if (!(is_local(config, &(connaddr->sin_addr)))) {
      show_msg(MSGDEBUG, "Connection for socket %d is local\n", __fd);
      return(realconnect(__fd, __addr, __len));
   }

   if (sock_type != SOCK_STREAM)  {
      show_msg(MSGDEBUG, "Connection isn't a TCP stream ignoring\n");
   }


   //TODO allow UDP filtering
      if (0 && (sock_type != SOCK_STREAM))  {
      errno = ECONNREFUSED;
      return(-1);
      show_msg(MSGDEBUG, "Connection isn't a TCP stream ignoring\n");
		  return(realconnect(__fd, __addr, __len));
   }

   /* If we haven't initialized yet, do it now */
   get_config();

   /* If the socket is already connected, just call connect  */
   /* and get its standard reply                             */
   if (!getpeername(__fd, (struct sockaddr *) &peer_address, &namelen)) {
      show_msg(MSGDEBUG, "Socket is already connected, defering to "
                         "real connect\n");
		return(realconnect(__fd, __addr, __len));
   }






   show_msg(MSGDEBUG, "app name %s - Got connection request for socket %d to "
                      "%s\n", global_app_name, __fd, inet_ntoa(connaddr->sin_addr));


  
   if(is_app_allowed(global_app_name)) {
     global_init_allowed = 1;
     show_msg(MSGDEBUG, "Allowing app %s \n", global_app_name);
     show_msg(MSGDEBUG, "Allowing app %s \n", program_invocation_name);
     return(realconnect(__fd, __addr, __len));
   }
   if(global_app_long_name[0] != NULL && is_app_allowed( global_app_long_name )) {
     global_init_allowed = 1;
     show_msg(MSGDEBUG, "Allowing app %s \n", global_app_long_name);
     show_msg(MSGDEBUG, "Allowing app %s \n", program_invocation_name);
     return(realconnect(__fd, __addr, __len));
   }

    show_msg(MSGDEBUG, "Refusing unauthorized app %s - %s \n", global_app_name, program_invocation_name);
    errno = ECONNREFUSED;
    return(-1);

}