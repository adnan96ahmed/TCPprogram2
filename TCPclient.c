#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>
#include <time.h>
 
int main(int argc, char *argv[])
{
    //argument variables
    int PORTNUM;
    char *stringPortNum;
    char *DEST_IP;
    int bufferLength = 4096;
    
    //verifying correct number of command lines and storing values into variables
    if (argc >= 2) {
        DEST_IP = strtok(argv[1], ":");
        stringPortNum = strtok(NULL, ":");
        PORTNUM = atoi(stringPortNum);
        if (argc == 4) {
            if ((atoi(argv[3]) >= 1) && (atoi(argv[3]) <= 4096)) {
                bufferLength = atoi(argv[3]);
            }
        }
        if (argc > 4) {
            printf("Please provide 'server-IP-address:port-number' as the second argument,\n the filename to read from as the third,\n and optionally a buffer length as the fourth argument\n");
            return -1;
        }
    } else {
        printf("Please provide 'server-IP-address:port-number' as the second argument,\n the filename to read from as the third,\n and optionally a buffer length as the fourth argument\n");
        return -1;
    }
    
    //opening file
    FILE *fp = NULL;
    fp = fopen(argv[2], "r");
    if (fp==NULL) {
        printf("Could not find/open the file\n");
        return -1;
    }
    
    //info for server, if connection is not local host
    struct addrinfo hints, *servinfo, *p;
    int rv;
    
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; // use AF_INET6 to force IPv6
    hints.ai_socktype = SOCK_STREAM;
    
    //int len;
    int mysocket;
	struct sockaddr_in dest; // socket info about the machine connecting to us
    
    if (strcmp(DEST_IP,"127.0.0.1")!=0) {
        
        //the following code in this part of the if statement is taken from: http://beej.us/guide/bgnet/output/html/multipage/getaddrinfoman.html
        
        if ((rv = getaddrinfo(DEST_IP, stringPortNum, &hints, &servinfo)) != 0) {
            fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
            exit(1);
        }
        
        // loop through all the results and connect to the first we can
        for(p = servinfo; p != NULL; p = p->ai_next) {
            if ((mysocket = socket(p->ai_family, p->ai_socktype,
                                 p->ai_protocol)) == -1) {
                perror("socket");
                continue;
            }
            
            if (connect(mysocket, p->ai_addr, p->ai_addrlen) == -1) {
                perror("connect");
                close(mysocket);
                continue;
            }
            break; // if we get here, we must have connected successfully
        }
        
        if (p == NULL) {
            // looped off the end of the list with no connection
            fprintf(stderr, "failed to connect\n");
            exit(2);
        }
        
        freeaddrinfo(servinfo); // all done with this structure
    } else {
        /* Create a socket.
         The arguments indicate that this is an IPv4, TCP socket
         */
        mysocket = socket(AF_INET, SOCK_STREAM, 0);
        
        memset(&dest, 0, sizeof(dest));                // zero the struct
        
        //Initialize the destination socket information
        dest.sin_family = AF_INET;                       // Use the IPv4 address family
        dest.sin_addr.s_addr = inet_addr(DEST_IP);
        dest.sin_port = htons(PORTNUM);                // Set destination port number
        
        // Connect to the server
        connect(mysocket, (struct sockaddr *)&dest, sizeof(struct sockaddr_in));
    }
    
    int count = 1;
    char *buffer;
    char fileSize[10];
    char chunkSize[10];
    size_t result;
    int size;
    buffer = (char*) malloc (sizeof(char)*bufferLength);
    
    send(mysocket, argv[2], strlen(argv[2]), 0); //sending file name
    //sending complete file size
    fseek(fp, 0, SEEK_END);
    size = ftell(fp);
    rewind(fp);
    sprintf(fileSize, "%d", size);
    sleep(2);
    //sending file size
    send(mysocket, fileSize, strlen(fileSize), 0); //sending file size
    sleep(2);
    //sending the chunk size
    sprintf(chunkSize, "%d", bufferLength);
    send(mysocket, chunkSize, strlen(chunkSize), 0); //sending chunk size
    sleep(2);
    
    //the following code for fread is taken from: http://www.cplusplus.com/reference/cstdio/fread/
    result = fread(buffer,1,bufferLength,fp);
    while (result == bufferLength) {
        send(mysocket, buffer, bufferLength, 0);
        printf("%d) Sent (%d) bytes\n", count, bufferLength);
        count++;
        result = fread(buffer,1,bufferLength,fp);
    }

    send(mysocket, buffer, result, 0);
    printf("%d) Sent (%zu) bytes\n", count, result);
    //send message indicating to the server that the entire message has been read
    /*
    strcpy(buffer, "*ended");
    send(mysocket, buffer, strlen(buffer), 0);
    */
    
    printf("Finished sending message\n");
    
    free(buffer);
    fclose(fp);
	close(mysocket);
    
	return EXIT_SUCCESS;
}
