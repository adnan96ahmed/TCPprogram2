#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>

//****************************** Message queue stuff ******************************
#include "queue.h"

//Create a queue and initilize its mutex and condition variable
MessageQueue* createMessageQueue()
{
    MessageQueue* q = (MessageQueue*)malloc(sizeof(MessageQueue));
    q->head = q->tail = NULL;
    pthread_mutex_init(&q->mutex, NULL);
    
    //Initialize the condition variable
    pthread_cond_init(&q->cond, NULL);
    return q;
}

//"Send" a message - append it onto the queue
void sendMessage(MessageQueue* q, int sender, char IP[15], char value[21])
{
    MessageNode* node = (MessageNode*)malloc(sizeof(MessageNode));
    node->msg.sender = sender;
    //node->msg.value = value;
    strcpy(node->msg.value, value);
    strcpy(node->msg.IP, IP);
    node->next = NULL;

    // critical section
    pthread_mutex_lock(&q->mutex);
    if (q->tail != NULL) {
        q->tail->next = node;       // append after tail
        q->tail = node;
    } else {
        q->tail = q->head = node;   // first node
    }
    //Signal the consumer thread woiting on this condition variable
    pthread_cond_signal(&q->cond);
    pthread_mutex_unlock(&q->mutex);
    //fprintf(stderr, "Enqueues the message for file: %s, signals cond variable, unlocks mutex, and goes to sleep\n", q->head->msg.value);
    sleep(2);

}

//"Receive" a message - remove it from the queue
int getMessage(MessageQueue* q, Message* msg_out)
{
    int success = 0;
    
    // critical section
    pthread_mutex_lock(&q->mutex);
    
    //Wait for a signal telling us that there's something on the queue
    //If we get woken up but the queue is still null, we go back to sleep
    while(q->head == NULL){
        fprintf(stderr,"Message queue is empty and getMessage goes to sleep (pthread_cond_wait)\n");
        pthread_cond_wait(&q->cond, &q->mutex);
        fprintf(stderr,"getMessage is woken up - someone signalled the condition variable\n");
    }
    
    //By the time we get here, we know q->head is not null, so it's all good
    MessageNode* oldHead = q->head;
    *msg_out = oldHead->msg;    // copy out the message
    q->head = oldHead->next;
    if (q->head == NULL) {
        q->tail = NULL;         // last node removed
    }
    free(oldHead);
    success = 1;
    
    //Release lock
    pthread_mutex_unlock(&q->mutex);

    return success;
}

void viewQueue(MessageQueue* q) {
    // critical section
    pthread_mutex_lock(&q->mutex);
    
    //Wait for a signal telling us that there's something on the queue
    //If we get woken up but the queue is still null, we go back to sleep
    while(q->head == NULL){
        fprintf(stderr,"Message queue is empty and getMessage goes to sleep (pthread_cond_wait)\n");
        pthread_cond_wait(&q->cond, &q->mutex);
        fprintf(stderr,"getMessage is woken up - someone signalled the condition variable\n");
    }
    
    while (q->head != NULL) {
        printf("Downloading file: %s\n", q->head->msg.value);
        q->head = q->head->next;
    }

    //Release lock
    pthread_mutex_unlock(&q->mutex);
}
//*****************************************************************************
