
/*
*Colton Rivard
*sec 021
*Lab6 memoryManager 
*/

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>

#include "memory_manager.h"
typedef struct node {
    int value;
    int free;
    struct node* next;
    struct node* prev;
    struct node* mem;

} node;

struct link{
    node *head;
};

static volatile struct link myMalloc;
pthread_mutex_t malloc_mutex = PTHREAD_MUTEX_INITIALIZER;




/*
 * Using static causes the compiler to
 * limit visibility of the varibles to this file only
 * This can be used to simulate 'private' variables in c
 */
static int allocation_count = 0; 

/* TODO Define additional structure definitions here */

/* mmInit()
 *     Initialize the memory manager to "manage" the given location
 *     in memory with the specified size.
 *         Parameters: start - the start of the memory to manage
 *                     size - the size of the memory to manage
 *         Returns: void
 */
void mmInit(void* start, int size) {
    if(myMalloc.head == NULL) {
        start = malloc(sizeof(node));
        myMalloc.head = start;
        struct node* temp = myMalloc.head;
        temp->value = size;
        pthread_mutex_init(&malloc_mutex,NULL);
        allocation_count = 0;

    }
}
// TODO more initialization needed

/* mmDestroy()
 *     Cleans up any storage used by the memory manager
 *     After a call to mmDestroy:
 *         all allocated spaces become invalid
 *         future allocation attempts fail
 *         future frees result in segmentation faults
 *     NOTE: after a call to mmDestroy a call to mmInit
 *           reinitializes the memory manager to allow allocations
 *           and frees
 *         Parameters: None
 *         Returns: void
 */
void mmDestroy()
{

    struct node* temp1;
    while (myMalloc.head != NULL) {
        temp1 = myMalloc.head;
        myMalloc.head = myMalloc.head->next;
        if(temp1->free == 1){
        free(temp1->mem);
        }
        free(temp1);
        

    }
    pthread_mutex_destroy(&malloc_mutex);

}

/* mymalloc_ff()
 *     Requests a block of memory be allocated using
 *         first fit placement algorithm
 *     The memory manager must be initialized (mmInit)
 *         for this call to succeed
 *         Parameters: nbytes - the number of bytes in the requested memory
 *         Returns: void* - a pointer to the start of the allocated space
 */
void* mymalloc_ff(int nbytes)
{

    pthread_mutex_lock(&malloc_mutex);
   
    int malloced = 0;
    struct node* temp1 = myMalloc.head;

    while (temp1 != NULL && !malloced) {
        if (myMalloc.head != NULL) {
            if (temp1->value >= nbytes && (temp1->free != 1)) {
                if(temp1->value != nbytes) {
                    struct node *newNode = malloc(sizeof(struct node));
                    newNode->value = temp1->value - nbytes;
                    newNode->next = NULL;
                    temp1->value = nbytes;
                    newNode->next = temp1->next;
                    temp1->next = newNode;
                    newNode->prev = temp1;
                }
                temp1->free = 1;
                malloced = 1;
            } else {
                temp1 = temp1->next;
            }

        }
    }


    if(temp1 != NULL){
        allocation_count++;
        temp1->mem = (node*) malloc(temp1->value* sizeof(int));


    } else{
        pthread_mutex_unlock(&malloc_mutex);
        return NULL;
    }


    pthread_mutex_unlock(&malloc_mutex);
    return temp1->mem;
}

/* mymalloc_wf()
 *     Requests a block of memory be allocated using
 *         worst fit placement algorithm
 *     The memory manager must be initialized (mmInit)
 *         for this call to succeed
 *         Parameters: nbytes - the number of bytes in the requested memory
 *         Returns: void* - a pointer to the start of the allocated space
 */
void* mymalloc_wf(int nbytes) {

    pthread_mutex_lock(&malloc_mutex);
  
    int malloced = 0;
    int largest = 0;
    struct node *temp1 = myMalloc.head;
    struct node *temp2 = myMalloc.head;
    while (temp2 != NULL) {
        if (temp2->value >= largest && temp2->free != 1) {
            largest = temp2->value;
        }
        temp2 = temp2->next;
    }

    while (temp1 != NULL && !malloced) {
        if (myMalloc.head != NULL) {
            if (temp1->value >= nbytes && temp1->free != 1 != 0 && temp1->value == largest) {
                if(temp1->value != nbytes) {
                    struct node *newNode = malloc(sizeof(struct node));
                    newNode->value = temp1->value - nbytes;
                    newNode->next = NULL;
                    temp1->value = nbytes;
                    newNode->next = temp1->next;
                    temp1->next = newNode;
                    newNode->prev = temp1;
                }
                temp1->free = 1;
                malloced = 1;
            } else {
                temp1 = temp1->next;
            }

        }
    }


    if(temp1 != NULL){
        allocation_count++;
        temp1->mem = (node*) malloc(temp1->value* sizeof(int));


    } else{
        pthread_mutex_unlock(&malloc_mutex);
        return NULL;
    }

    pthread_mutex_unlock(&malloc_mutex);
    return temp1->mem;
}


/* mymalloc_bf()
 *     Requests a block of memory be allocated using
 *         best fit placement algorithm
 *     The memory manager must be initialized (mmInit)
 *         for this call to succeed
 *         Parameters: nbytquites - the number of bytes in the requested memory
 *         Returns: void* - a pointer to the start of the allocated space
 */
void* mymalloc_bf(int nbytes) {

    pthread_mutex_lock(&malloc_mutex);
   
    int malloced = 0;
    int largest = 0;
    int smallest = myMalloc.head->value;
    struct node *temp1 = myMalloc.head;
    struct node *temp2 = myMalloc.head;
    struct node *temp3 = myMalloc.head;
    while (temp3 != NULL) {
        if (temp2->value >= largest && temp3->free != 1) {
            largest = temp3->value;
            smallest = largest;
        }
        temp3 = temp3->next;
    }
    while (temp2 != NULL) {
        if ((temp2->value <= smallest) && (temp2->free != 1) &&( nbytes < temp2->value)) {
            smallest = temp2->value;
        }
        temp2 = temp2->next;
    }

    while (temp1 != NULL && !malloced) {
        if (temp1 != NULL) {
            if (temp1->value >= nbytes && (temp1->free != 1) && (temp1->value == smallest)) {
                if(temp1->value != nbytes) {
                    struct node *newNode = malloc(sizeof(struct node));
                    newNode->value = temp1->value - nbytes;
                    newNode->next = NULL;
                    temp1->value = nbytes;
                    newNode->next = temp1->next;
                    temp1->next = newNode;
                    newNode->prev = temp1;
                }
                temp1->free = 1;
                malloced = 1;
            } else {
                temp1 = temp1->next;
            }

        }
    }


    if(temp1 != NULL){
        allocation_count++;
        temp1->mem = (node*) malloc(temp1->value* sizeof(int));


    } else{
        pthread_mutex_unlock(&malloc_mutex);
        return NULL;
    }

    pthread_mutex_unlock(&malloc_mutex);
    return temp1->mem;
}
/* myfree()
 *     Requests a block of memory be freed and the storage made
 *         available for future allocations
 *     The memory manager must be initialized (mmInit)
 *         for this call to succeed
 *         Parameters: ptr - a pointer to the start of the space to be freed
 *         Returns: void
 *         Signals a SIGSEGV if a free is not valid
 *             - memory manager is not initialized
 *             - memory manager has been destroyed
 *             - ptr is not allocated (e.g. double free)
 */
void myfree(void* ptr)
{

    pthread_mutex_lock(&malloc_mutex);
    pid_t processId = getpid();
    if(myMalloc.head == NULL){
     printf("mmemory manager has been destroyed");
     kill(processId,SIGSEGV);
    }
    struct node* temp = myMalloc.head;
    if(myMalloc.head == NULL){
    mmDestroy();
    kill(processId,SIGSEGV);
    }
    while (temp != NULL){
        if (ptr == temp->mem) {
            if(temp->free == 1) {
               free(temp->mem);
               temp->free = 0;
               
            } else {
            	printf("ptr is not allocated");
                mmDestroy();
                kill(processId,SIGSEGV);
            }

        }
        temp = temp->next;
    }
    pthread_mutex_unlock(&malloc_mutex);

}


/* get_allocated_space()
 *     Retrieve the current amount of space allocated by the memory manager (in bytes)
 *         Parameters: None
 *         Returns: int - the current number of allocated bytes
 */
int get_allocated_space()
{
    pthread_mutex_lock(&malloc_mutex);
    int allocated_space = sizeof(myMalloc);
    pthread_mutex_unlock(&malloc_mutex);
    return allocated_space;

}

/* get_remaining_space()
 *     Retrieve the current amount of available space in the memory manager (in bytes)
 *         (e.g. sum of all free blocks)
 *         Parameters: None
 *         Returns: int - the current number of free bytes
 */
int get_remaining_space()
{
    pthread_mutex_lock(&malloc_mutex);
    int remainingSpace = 0;
    struct node* temp1 = myMalloc.head;

    while(temp1 != NULL){
        if(temp1->free == 0){
            remainingSpace += temp1->value;
        }
        temp1 = temp1->next;
    }
    pthread_mutex_unlock(&malloc_mutex);
    return remainingSpace;

    return 0;
}

/* get_fragment_count()
 *     Retrieve the current amount of free blocks (i.e. the count of all the block, not the size)
 *         Parameters: None
 *         Returns: int - the current number of free blocks
 */
int get_fragment_count()
{
    pthread_mutex_lock(&malloc_mutex);
    struct node* temp1 = myMalloc.head;
    int fragment = 0;
    while(temp1 != NULL){
        if(temp1->free == 0){
            fragment++;
        }
        temp1 = temp1->next;
    }
    pthread_mutex_unlock(&malloc_mutex);
    return fragment;
}

/* get_mymalloc_count()
 *     Retrieve the number of successfull malloc calls (for all placement types)
 *         Parameters: None
 *         Returns: int - the total number of successfull mallocs
 */
int get_mymalloc_count()
{
    pthread_mutex_lock(&malloc_mutex);
    return allocation_count;
    pthread_mutex_unlock(&malloc_mutex);
}
