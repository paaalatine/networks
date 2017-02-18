#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define SERVERPORT "3490"    // порт для соединения
#define MAXDATASIZE 64000

int main(int argc, char *argv[])
{
    int sent;
    int sockfd;
    struct addrinfo hints, *servinfo, *p;
    int rv;
    int numbytes;
    char c[MAXDATASIZE];
    FILE* in;

    struct timeval ta, te;

    gettimeofday(&ta, NULL);

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;

    if ((rv = getaddrinfo("127.0.0.1", SERVERPORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %sn", gai_strerror(rv));
        return 1;
    }

    // пробегаемся по результатам и создаём сокет
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("talker: socket");
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "talker: failed to bind socketn");
        return 2;
    }

    in = fopen("test1.txt","rb");

    while(!feof(in))
    {
        memset(c, 0, sizeof(char)*MAXDATASIZE);
        int b=fread(c,1,MAXDATASIZE,in);
        if ((numbytes = sendto(sockfd, c, sizeof(c), 0,
             p->ai_addr, p->ai_addrlen)) == -1) {
        perror("talker: sendto");
        exit(1);
        }
        printf("sent\n");
        sent++;
    }
    freeaddrinfo(servinfo);
    gettimeofday(&te, NULL);

    printf("time elapsed: %lf sec\n", te.tv_sec - ta.tv_sec + (te.tv_usec - ta.tv_usec)/1000000.0);
    printf("number of sent packs: %d\n", sent);
    fclose(in);
    close(sockfd);

    return 0;
}