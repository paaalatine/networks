/*
** client.c -- a stream socket client demo
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <arpa/inet.h>

#define PORT "3490" // Порт, к которому подключается клиент

#define MAXDATASIZE 64000// максимальное число байт, принимаемых за один раз

// получение структуры sockaddr, IPv4 или IPv6:

void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(int argc, char *argv[])
{
    int sockfd, numbytes;  
    char buf[MAXDATASIZE];
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char s[INET6_ADDRSTRLEN];
    FILE* out;

    struct timeval ta, te;

	gettimeofday(&ta, NULL);

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo("localhost", PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %sn", gai_strerror(rv));
        return 1;
    }

    // Проходим через все результаты и соединяемся к первому возможному
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("client: socket");
            continue;
        }

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("client: connect");
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "client: failed to connect\n");
        return 2;
    }

    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
            s, sizeof s);
    printf("client: connecting to %s\n", s);

    freeaddrinfo(servinfo); // эта структура больше не нужна
    while (1){

    	if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) < 0) {
        perror("recv");
        exit(1);
    }

        if(numbytes == 0)
        	break;
        
        out = fopen("testres.txt","a");
        fwrite(buf, sizeof(buf), 1, out);
        fclose(out);
    }

    printf("client: received\n");
    
    gettimeofday(&te, NULL);

	printf("time elapsed: %lf sec\n", te.tv_sec - ta.tv_sec + (te.tv_usec - ta.tv_usec)/1000000.0);

    close(sockfd);

    return 0;
}