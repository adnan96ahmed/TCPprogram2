#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

//****************************** Message queue stuff ******************************
typedef struct {
    int sender;
    char value[21];
    char IP[15];
} Message;

//Message node
typedef struct message_node {
    Message msg;
    struct message_node* next;
} MessageNode;

//Message queue - a singly linked list
//Remove from head, add to tail
typedef struct {
    MessageNode* head;
    MessageNode* tail;
    pthread_mutex_t mutex;
    
    //Add a condition variable
    pthread_cond_t cond;
} MessageQueue;

//Create a queue and initilize its mutex and condition variable
MessageQueue* createMessageQueue();

//"Send" a message - append it onto the queue
void sendMessage(MessageQueue* q, int sender, char IP[15], char value[21]);

//"Receive" a message - remove it from the queue
int getMessage(MessageQueue* q, Message* msg_out);

void viewQueue(MessageQueue* q);

//*****************************************************************************
