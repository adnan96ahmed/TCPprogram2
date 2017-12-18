#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <pthread.h>
#include <stdint.h>
#include "queue.h"

#define MAXRCVLEN 4096

// flag shared by all threads
int g_Flag = 0;

// mutex used to protect the flag
pthread_mutex_t g_Mutex;

void* threadFunc(void *arg);
void* uiThread(void *arg);

//Each thread needs multiple arguments, so we create a dedicated struct
typedef struct {
    int workerId;
    char IP[15];
    MessageQueue* q;
} ThreadArg;

int main(int argc, char *argv[])
{
    int PORTNUM;
    pthread_t tid;
    
    if (argc == 2) {
        PORTNUM = atoi(argv[1]);
    } else {
        printf("Please only provide the port number as the second argument\n");
        return -1;
    }
  
    // initialize the mutex
    pthread_mutex_init(&g_Mutex, NULL);
    
	struct sockaddr_in dest; // socket info about the machine connecting to us
	struct sockaddr_in serv; // socket info about our server
	int mysocket;            // socket used to listen for incoming connections
	socklen_t socksize = sizeof(struct sockaddr_in);

	memset(&serv, 0, sizeof(serv));           // zero the struct before filling the fields
	
	serv.sin_family = AF_INET;                // Use the IPv4 address family
	serv.sin_addr.s_addr = htonl(INADDR_ANY); // Set our address to any interface 
	serv.sin_port = htons(PORTNUM);           // Set the server port number 

	/* Create a socket.
   	   The arguments indicate that this is an IPv4, TCP socket
	*/
	mysocket = socket(AF_INET, SOCK_STREAM, 0);
  
	// bind serv information to mysocket
    if ((bind(mysocket, (struct sockaddr *)&serv, sizeof(struct sockaddr)) == -1)) {
        fprintf( stderr, "Server could not bind to port number: %d\vn", PORTNUM );
        return -1;
    }

	// start listening, allowing a queue of up to 1 pending connection
	listen(mysocket, 1);
    ThreadArg arg;
    MessageQueue *q = createMessageQueue();
	
    // Create a socket to communicate with the client that just connected
    int consocket = accept(mysocket, (struct sockaddr *)&dest, &socksize);
    //printf("Incoming connection from %s \n", inet_ntoa(dest.sin_addr));
    
    arg.workerId = consocket;
    arg.q = q;

    //creating UI thread
    pthread_create(&tid, NULL, uiThread, &arg);
    
    //got solution to cast int to void pointer from: https://stackoverflow.com/questions/8487380/how-to-cast-an-integer-to-void-pointer
    while(consocket && g_Flag == 0) {
        // create a transfer thread
        //pthread_create(&tid, NULL, threadFunc, (void *) (intptr_t) consocket);
        pthread_create(&tid, NULL, threadFunc, &arg);
        
        if (g_Flag == 1) {//if UI thread tells main thread to terminate program
            exit(1);
        }
        
        //Continue listening for incoming connections
        consocket = accept(mysocket, (struct sockaddr *)&dest, &socksize);
        arg.workerId = consocket;
        strcpy(arg.IP, inet_ntoa(dest.sin_addr));
        //arg.workerId = consocket;
    }

    printf("ending program\n");
	close(mysocket);
	return EXIT_SUCCESS;
}

void *threadFunc(void *arg)
{
    FILE *fp = NULL;
    int len;
    int fileSize;
    int chunkSize;
    int count = 0;
    char buffer[MAXRCVLEN + 1]; // +1 so we can add null terminator
    //int theConsocket = (intptr_t) arg;
    ThreadArg *theArg = (ThreadArg*)arg;
    int theConsocket = theArg->workerId;
    
    //printf("Starting the thread function\n");
    
    // Receive file name from the client
    len = recv(theConsocket, buffer, MAXRCVLEN, 0);
    buffer[len] = '\0';
    
    fp = fopen(buffer, "w+");
    sendMessage(theArg->q, theArg->workerId, theArg->IP, buffer);
    
    // Receive file size from the client
    len = recv(theConsocket, buffer, MAXRCVLEN, 0);
    buffer[len] = '\0';
    fileSize = atoi(buffer);
    
    // Receive chunk size from the client
    len = recv(theConsocket, buffer, MAXRCVLEN, 0);
    buffer[len] = '\0';
    chunkSize = atoi(buffer);
    
    while (count < fileSize) {
        len = recv(theConsocket, buffer, chunkSize, 0);
        buffer[len] = '\0';
        fprintf(fp, "%s", buffer);
        count = count + chunkSize;
    }
    
    fclose(fp);
    //Close current connection
    close(theConsocket);
    
    Message msg;
    if (getMessage(theArg->q, &msg)) {
        fprintf(stderr,"getMessage removes message of file: %s\n", msg.value);
    }
    
    printf("Ending the thread function\n");
    return NULL;
}

void *uiThread(void *arg) {
    
    // critical section
    pthread_mutex_lock(&g_Mutex);
    
    ThreadArg *theArg = (ThreadArg*)arg;
    char input = '0';
    
    printf("Select the number of one of the numbers below:\n");
    printf("1 to display active transfers\n");
    printf("2 to shut down the server\n");

    while (input != '2') {
        input = getchar();
        if (input == '1') {
            viewQueue(theArg->q);
        }
    }
    
    g_Flag = 1;
    
    //Release lock
    pthread_mutex_unlock(&g_Mutex);
    
    printf("Ending the ui thread function\n");
    sleep(1);
    exit(1);
    return NULL;
}

