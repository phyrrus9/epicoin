#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include "epihead.h"

int main(int argc, char *argv[])
{
	int sockfd = 0, n = 0;
	char recvBuff[1024];
	struct sockaddr_in serv_addr;
	FILE *f = fopen(argv[2], "r");
	int flen = 0;
	char tmpc;

	while (fscanf(f, "%c", &tmpc) != EOF) flen++;
	fclose(f);

	memset(recvBuff, '0',sizeof(recvBuff));
	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf("\n Error : Could not create socket \n");
		return 1;
	}

	memset(&serv_addr, '0', sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(5000);

	if(inet_pton(AF_INET, argv[1], &serv_addr.sin_addr)<=0)
	{
		printf("\n inet_pton error occured\n");
		return 1;
	}

	if( connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
	{
		printf("\n Error : Connect Failed \n");
		return 1;
	}

	struct _epi_header cliheader;
	struct _server_response servr;
	strcpy(cliheader.name, "Test");
	strcpy(cliheader.password, "PasswordTest");
	cliheader.flags = (1 << 1 | 1 << 4);
	cliheader.pilen = flen;
	cliheader.elen = flen;
	cliheader.epilen = flen;

	write(sockfd, &cliheader, sizeof(_epi_header));
	read(sockfd, &servr, sizeof(_server_response));
	f = fopen(argv[2], "r");
	while (fscanf(f, "%c", &tmpc) != EOF)
		write(sockfd, &tmpc, 1);
	close(sockfd);

	return 0;
}
