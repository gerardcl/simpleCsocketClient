/**
* rc.c - remote control by simple socket messaging
* 
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

int
main(int argc, char *argv[]) {
	int sockfd, numbytes;
	long port;
    char *msg = NULL;

    char buf[MAXDATASIZE];
    struct hostent *host;
    struct sockaddr_in their_addr;

    if (argc != 4) {
        fprintf(stderr, "Usage: %s to_host to_port \"message\"\n",
            argv[0]);
        exit(1);
    }

    if ((host = gethostbyname(argv[1])) == NULL ) {
        perror("gethostbyname()");
        exit(1);
    } else
        printf("remote host is: %s\n", argv[1]);

    if (argv[2] == NULL || argv[2]<2) {
        perror("no port...");
        exit(1);
    } else {
        port = strtol(argv[2], NULL, 10);;
        printf("remote host port to send: %d\n", port);
    }

    if (argv[3] == NULL ) {
        perror("no message specified...");
        exit(1);
    } else {
        printf("message to send: %s\n", argv[3]);
        msg = malloc(strlen(argv[3]));
        strcpy(msg, argv[3]);
        strcat(msg, "\r\n");
    }

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket()");
        exit(1);
    } else
        printf("socket() sockfd is OK...\n");

    their_addr.sin_family = AF_INET;
    printf("sending message to host %s and port %d...\n", argv[1], port);
    their_addr.sin_port = htons(port);
    their_addr.sin_addr = *((struct in_addr *) host->h_addr);
    memset(&(their_addr.sin_zero), '\0', 8);

    if (connect(sockfd, (struct sockaddr *) &their_addr,
        sizeof(struct sockaddr)) == -1)
    {
        perror("connect()");
        exit(1);
    } else {
        printf("connect() is OK...\n");
        int rc = send(sockfd, msg, strlen(msg)+strlen("\r\n"), 0);
        printf("sending msg: %s\nsento() result = %d\n", msg, rc);

        if (rc < 0) {
            close(sockfd);
            exit(1);
        }
    }

    if ((numbytes = recv(sockfd, buf, MAXDATASIZE - 1, 0)) == -1) {
        perror("recv()");
        exit(1);
    } else
        printf("recv() is OK...\n");

    buf[numbytes] = '\0';
    printf("received message: %s\n", buf);

    printf("closing sockfd\n");
    close(sockfd);
	free(msg);
    return 0;
}
