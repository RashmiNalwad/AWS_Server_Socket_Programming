/*
** client.c -- a stream socket client demo
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

#include <arpa/inet.h>

#define TCPPORT "25341" // TCP port client will be connecting to 

#define MAXDATASIZE 50000 // max number of bytes we can get at once 

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)                      //Beej Guide reference starts
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

    FILE *ifp, *ofp;
    char *mode = "r";
    int num[1000];

    int *arr = malloc(sizeof(int) * 1000);

    int i=0;
    int count = 1000;

    if (argc != 2) {
        fprintf(stderr,"usage: Function name { min, max, sum, sos}\n");
        exit(1);
    }

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo("localhost", TCPPORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and connect to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("client: socket");
            continue;
        }

        printf("%s\n", "The client is up and running");

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

    //freeaddrinfo(servinfo); // all done with this structure


   //send client port to Server.

    if (send(sockfd, argv[1], 5, 0) == -1) //send name function to         //Beej Guide reference ends.  
        perror("send");

    printf("The client has sent the reduction type %s to AWS\n",argv[1]);

    //sending numbers to server
    ifp = fopen("nums.csv", mode);

    if (ifp == NULL) {
       fprintf(stderr, "Can't open input file in.list!\n");
       exit(1);
    }

    uint32_t network_byte_order;

    int numCount =0;

    while(fscanf(ifp,"%d",&arr[i])!=EOF)
    {
        numCount++;
        i++;
    }

    int lkj;
    lkj = 0;


    //Send total numbers read from file to server.
    network_byte_order = htonl(numCount);                                   
    if(write(sockfd, &network_byte_order,sizeof(network_byte_order)) < 1)
         perror("Number : Write to server failed");

    i =0;

    //Send numbers to server.
    while(lkj < numCount)
    {
        network_byte_order = htonl(arr[i]);
        //printf("%d\n",arr[i]);

        if(write(sockfd, &network_byte_order,sizeof(network_byte_order)) < 1)
            perror("Write to server failed");
        lkj++;
        i++;
    }
    printf("The client has sent %d numbers to AWS\n", numCount);
    fclose(ifp); 

    // Receive final number. First receive
    if ((numbytes = recv(sockfd, buf, MAXDATASIZE, 0)) == -1) {
        perror("recv");
        exit(1);
    }
    buf[numbytes] = '\0';
    printf("The client has received the reduction %s: %s\n", argv[1],buf);
    close(sockfd);

    return 0;
}
