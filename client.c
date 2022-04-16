/* Andre Augusto Giannotti Scota (a2gs)                              
 * andre.scota@gmail.com
 *
 * A C TCPIPv6 client (with DNS resolution)
 *
 * MIT License
 *
 */

/* client.c
 *
 *  Who     | When       | What
 *  --------+------------+----------------------------
 *   a2gs   | 01/01/2005 | Creation
 *          |            |
 */


#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <signal.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "sc.h"

#define STRADDR_SZ	(50) /* max size IPv6 number format */

int main(int argc, char *argv[])
{
	struct addrinfo hints, *res = NULL, *rp = NULL;
	int errGetAddrInfoCode = 0, errConnect = 0;
	int sockfd = 0;
	char msg[MAXLINE] = {0};
	char strAddr[STRADDR_SZ + 1] = {'\0'};
	void *pAddr = NULL;

	if(argc != 3){
		fprintf(stderr, "%s IPv6_DNS_ADDRESS PORT\n", argv[0]);
		return(-1);
	}

	signal(SIGPIPE, SIG_IGN);

	memset (&hints, 0, sizeof (hints));
	hints.ai_family = AF_INET6; /* Forcing IPv4. The best thing to do is specify: AF_UNSPEC (IPv4 and IPv6 servers support) */
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags |= AI_CANONNAME | AI_ADDRCONFIG; /* getaddrinfo() AI_ADDRCONFIG: This flag is useful on, for example, IPv4-only  systems, to ensure that getaddrinfo() does not return IPv6 socket addresses that would always fail in connect(2) or bind(2) */

	errGetAddrInfoCode = getaddrinfo(argv[1], argv[2], &hints, &res);
	if(errGetAddrInfoCode != 0){
		printf("ERRO: getaddrinfo() [%s].\n", gai_strerror(errGetAddrInfoCode));
		return(-1);
	}

	for(rp = res; rp != NULL; rp = rp->ai_next){
		sockfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		if (sockfd == -1){
			printf("ERRO: socket() [%s].\n", strerror(errno));
			continue;
		}

		if(rp->ai_family == AF_INET)       pAddr = &((struct sockaddr_in *) rp->ai_addr)->sin_addr;
		else if(rp->ai_family == AF_INET6) pAddr = &((struct sockaddr_in6 *) rp->ai_addr)->sin6_addr;
		else                               pAddr = NULL;

		inet_ntop(rp->ai_family, pAddr, strAddr, STRADDR_SZ);
		printf("Trying connect to [%s/%s:%s].\n", rp->ai_canonname, strAddr, argv[2]);

		errConnect = connect(sockfd, rp->ai_addr, rp->ai_addrlen);
		if(errConnect == 0)
			break;

		printf("ERRO: connect() to [%s/%s:%s] [%s].\n", rp->ai_canonname, strAddr, argv[2], strerror(errno));

		close(sockfd);
	}

	if(res == NULL || errConnect == -1){ /* End of getaddrinfo() list or connect() returned error */
		printf("ERRO: Unable connect to any address.\n");
		return(-1);
	}

	freeaddrinfo(res);

	for(;;){
		memset(msg, 0, MAXLINE);

		memcpy(msg, "ok", 2);

		if(send(sockfd, msg, 2, 0) == -1){
			printf("ERRO: send() [%s].\n", strerror(errno));
			break;
		}
		sleep(2);

	}

	shutdown(sockfd);
	return(0);
}
