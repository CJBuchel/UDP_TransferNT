#ifndef NT_HEADERS
#define NT_HEADERS

#ifdef __linux__

#include <iostream>

// Server headers
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <tuple>

#endif // __linux__

// Threading
#include <thread>

/**
 * Network values
 */

/**
 * Teamnumber
 */
#ifndef TEAM

	/**
	 * Use direct connect to ip
	 */
	#ifndef SERVER_IP
	#define SERVER_IP "127.0.0.1"
	#endif
#endif


/**
 * IP Length
 */
#ifndef IP_LEN
#define IP_LEN 20
#endif

#define NETWORK_VERSION "1.4"

/**
 * Buffer size for data transfer
 */
#ifndef DEFAULT_BUFFSIZE
#define DEFAULT_BUFFSIZE 512 // Max length of buffer
#endif

/**
 * Port used to handshake with client to determin buffersize/packetSize
 */
#ifndef HANDSHAKE_PORT
#define HANDSHAKE_PORT 5800
#endif

/**
 * Port number for sending data
 */
#ifndef PORT
#define PORT 5801
#endif



/**
 * 
 * Network macros
 * 
 */

/**
 * Kill program with error message
 */
#define KILL(s) perror(s); exit(1)

/**
 * std error with program exit
 */
#define ERROR(s) fprintf(stderr, s); exit(1)


/**
 * std error without program exit
 */
#define ERROR_PRINT(s) perror(s); printf("\n")

#endif // NT_HEADER