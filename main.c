#include <stdio.h>
#include <stdlib.h>
#include "ArrayADT.h"
//#include "MutexSim.h"
#define quantumCheck 50

void starvationChecking (Priority_queue_p thePriorityQ);


int main(int argc, char** argv) {
    
    Priority_queue_p priority_Queue = createPQ();
    int totalRunTime = 0;
    
    PCB_p testingPCB1 = createPCB("PCB1", 1, 1, 1, 1, ready); 
    PCB_p testingPCB2 = createPCB("PCB2", 1, 1, 1, 1, ready); 
    PCB_p testingPCB3 = createPCB("PCB3", 1, 1, 1, 1, ready); 
    PCB_p testingPCB4 = createPCB("PCB4", 1, 1, 1, 1, ready); 
    PCB_p testingPCB5 = createPCB("PCB5", 1, 0, 1, 1, ready); 
    
    testingPCB1->term_count = 1;
    testingPCB2->term_count = 1;
    testingPCB3->term_count = 1;
    testingPCB4->term_count = 1;
    
    addPCB(priority_Queue, testingPCB1);
    addPCB(priority_Queue, testingPCB2);
    addPCB(priority_Queue, testingPCB3);
    addPCB(priority_Queue, testingPCB4);
    addPCB(priority_Queue, testingPCB5);
    
    //printList(priority_Queue->Queue_List[1]);
    
    // Inside a while loop, we check the starvation prevention when quantumCheck hit 50 for example
    //if(totalRunTime == quantumCheck) {
    starvationChecking (priority_Queue);
    //}
    
    freePriority_queue(priority_Queue);
    
    //printf("Hello world");
    
    return (EXIT_SUCCESS);
}

void starvationChecking (Priority_queue_p thePriorityQ) {
    
    int i = 0;
    struct node * tempNode;
    int totalOfPCBs = size_of(thePriorityQ);
    FIFO_queue_p tempFifoQueue = createLinkedList();
    
    // Loop through the priority q and add all the PCBs into a fifo q
    for(i = 0; i < totalOfPCBs; i++ ) {
        enqueue(tempFifoQueue, search(thePriorityQ));
    }
    
    tempNode = tempFifoQueue->frontNode_ptr;
    
    while (!is_empty(tempFifoQueue)) {
        
        if(tempNode->data->runTimes == 0 && tempNode->data->priority != 0) {
            tempNode->data->priority--;
        } else {
            tempNode->data->priority = tempNode->data->tempPriority;
        }
        
        tempNode->data->runTimes = 0;
        
        addPCB(thePriorityQ, dequeue(tempFifoQueue)->data);
        tempNode = tempNode->next;
    }
    
    //printList(thePriorityQ->Queue_List[1]);
    
    printf("\nThe size of the priority q is : %d\n", size_of(thePriorityQ));
    printf("The size of the fifo q is : %d\n", size_is(tempFifoQueue));
    freeLinkedList(tempFifoQueue);
}