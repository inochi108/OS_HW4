#ifndef LINKEDLISTADT_H
#define LINKEDLISTADT_H

#include "PCB.h"


/*
 ===========================================================================
 Name        : 422_HW3.c
 Date        : 2/12/2016
 Authors     : Nok Him Nam, Jowy Tran, Mark Johnson, David Humphreys
 Description : Discontinuities with I/O service request traps and interrupts
 ===========================================================================
 */

typedef struct FIFO_queue * FIFO_queue_p;

struct node {
	PCB_p data;
	struct node* next;
};

struct FIFO_queue {
	struct node * frontNode_ptr;
	struct node * endNode_ptr;
	int size;
};

FIFO_queue_p createLinkedList(void);
void createRandomPCB(FIFO_queue_p theList, int theNumberOfPCB);
int is_empty(FIFO_queue_p l);
void enqueue(FIFO_queue_p l, PCB_p data);
struct node * dequeue(FIFO_queue_p l);
void printList(FIFO_queue_p l);
void printListFileVersion(FIFO_queue_p theList, FILE* theOuputFile);
void freeLinkedList(FIFO_queue_p l);
int size_is(FIFO_queue_p theList);


#endif



