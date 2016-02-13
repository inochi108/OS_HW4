/*
 ===========================================================================
 Name        : 422_HW3.c
 Date        : 2/12/2016
 Authors     : Nok Him Nam, Jowy Tran, Mark Johnson, David Humphreys
 Description : Discontinuities with I/O service request traps and interrupts
 ===========================================================================
 */

/*                      NOTE:
=====================================================================
Don't Terminate PCBs anywhere, should be done in function call
A process termination print out would look like:
Process terminated: PID 334 at <system time>
Where system time here is the termination time for the process.
A timer interrupt would look like:
Timer interrupt: PID 22 was running, PID 425 dispatched
An I/O Completion interrupt would look like:
I/O completion interrupt: PID 34 is running, PID 667 put in ready queue
An I/O trap request would look like:
I/O trap request: I/O device 1, PID 4761 put into waiting queue, PID 387 dispatched
================================================================================
*/

#include <pthread.h>
#include <stdio.h>
#include "PCB.h"
#include "ArrayADT.h"
#include <time.h>
#include <unistd.h> // for usleep
#include <signal.h>
#include <errno.h>

//Number of PCBs to create
int const TOTAL_PCB_COUNT = 20;

//Number of instructions to execute before program ends
int const INSTRUCTIONS_TO_EXE = 200;

//Stops the program 4 times per quantum for this many milliseconds
int const PROGRAM_SPEED = 1;

//Timer wait time in milliseconds
int const TIMER_WAIT_TIME = 50;

//IO 1 wait time in milliseconds
int const IO1_WAIT_TIME = 186;

//IO 2 wait time in milliseconds
int const IO2_WAIT_TIME = 218;

pthread_mutex_t timer_mtx, io1_mtx, io2_mtx = PTHREAD_MUTEX_INITIALIZER;

/* TIMER FUNCTION*/
void *timer_function(void *int_void_p) {
    int programKill = (int) int_void_p;
    
    //Timer Loop
    while(programKill) {
        Sleep(TIMER_WAIT_TIME);
        pthread_mutex_lock(&timer_mtx);
    }

    /* the function must return something - NULL will do */
    return NULL;
}

void *io_1_function(void *int_void_p) {
    int programKill = (int) int_void_p;
    
    //IO1 Loop
    while(programKill) {
        Sleep(IO1_WAIT_TIME);
        pthread_mutex_lock(&io1_mtx);
        
    }

    /* the function must return something - NULL will do */
    return NULL;
}

void *io_2_function(void *int_void_p) {
    int programKill = (int) int_void_p;
    
    //IO2 Loop
    while(programKill) {
        Sleep(IO2_WAIT_TIME);
        pthread_mutex_lock(&io2_mtx);
    }

    /* the function must return something - NULL will do */
    return NULL;
}

/*Timer_ISR changes the state of the running process and does a context switch*/
void timer_ISR(Priority_queue_p PQ_p, PCB_p runningPCB_p) {
    //Currently unused as it kept causing unknown seg faults and junk data...
}

int main(int argc, char **argv) {
    
    FILE* output_ptr;
    if(argc == 2 && strlen(argv) > 0) {
        output_ptr = fopen(argv[1], "w+");
    }else {
        output_ptr = stdout;
    }
   
    //holds initial PCBs
    FIFO_queue_p new_queue = createLinkedList();
    
    //Print generated PCBs
    fprintf(output_ptr, "--------LIST OF GENERATED PCBs--------\n\n");
    
    //populates new_queue with PCBs
    createRandomPCB(new_queue, TOTAL_PCB_COUNT);
    
    fprintf(output_ptr, "----------PCB CREATION TIMES----------\n\n");
    
    //A Priority Queue to hold the organized PCBs in the ready state
    Priority_queue_p PQ_p = createPQ();
    
    //Queues that represent the two io waiting queues
    FIFO_queue_p io1_queue = createLinkedList();
    FIFO_queue_p io2_queue = createLinkedList();
    
    //runningPCB_p: the currently running pcb, temp: used for switching
    PCB_p runningPCB_p, temp;
    
    //A temp node that is used for returning from dequeued FIFO queues
    struct node* tempNode;
    
    //ends all running while loops in threads when set to 0
    int quantumNum = 1, i = 0, mutexCheck;

    //All PCBs go into ArrayADT and print their creation time
    while(size_is(new_queue) != 0) {
        tempNode = dequeue(new_queue);
        PCB_p tempPCB_p = tempNode->data;
        fprintf(output_ptr, "Process created: PID %d at %s\n", tempPCB_p->pid, tempPCB_p->creation);
        addPCB(PQ_p, tempPCB_p);
    }
    
    // Start the Idle PCB and initialize runningPCB to idle
    runningPCB_p = createIdlePCB();
    addPCB(PQ_p, createIdlePCB());

    //Threads initialization
    pthread_t timer_thread, io_1_thread, io_2_thread;
    
    pthread_mutex_init(&timer_mtx, NULL);
    pthread_mutex_init(&io1_mtx, NULL);
    pthread_mutex_init(&io2_mtx, NULL);

    /* create timer thread */
    if(pthread_create(&timer_thread, NULL, timer_function, &quantumNum)) {
    
        fprintf(output_ptr, "Error 326485: creating timer thread\n");
        return 3;
    }

    /* create io 1 thread */
    if(pthread_create(&io_1_thread, NULL, io_1_function, &quantumNum)) {

	fprintf(output_ptr, "Error 78787: creating io 1 thread\n");
	return 1;
    }

    /* create io 2 thread */
    if(pthread_create(&io_2_thread, NULL, io_2_function, &quantumNum)) {

	fprintf(output_ptr, "Error 666: creating io 2 thread\n");
	return 2;
    }
    
    fprintf(output_ptr, "------------------------------------\n\n");
    
    fprintf(output_ptr, "CPU STARTS RUNNING!\n");
    //CPU THREAD (loop) STARTS HERE! (all other threads are already running)
    /* CPU essentially runs until programKill is 0. */

    while(quantumNum != INSTRUCTIONS_TO_EXE) {
        
        fprintf(output_ptr, "\n------------QUANTUM #%d!------------\n", quantumNum);
        
        //Program delay speed in milliseconds
        Sleep(PROGRAM_SPEED);
        
        //try mutex lock on timer
        mutexCheck = pthread_mutex_trylock(&timer_mtx);

        //if timer mutex is already locked
        if(mutexCheck != 0) {
            
            //PSEUDO CALL TO TIMER_ISR
            runningPCB_p->currentState = ready;
            //PSEUDO CALL TO DISPATCHER
            temp = runningPCB_p;
            runningPCB_p = search(PQ_p);
            runningPCB_p->currentState = running;
            addPCB(PQ_p, temp);
            
            //print switching details
            fprintf(output_ptr, "\nTimer interrupt: PID %d was running, PID %d dispatched                    <- TIMER EVENT\n",
                    temp->pid, runningPCB_p->pid);
            
            pthread_mutex_unlock(&timer_mtx);
        }
        //if there was no lock already, unlock the lock we accomplished
        else {
            pthread_mutex_unlock(&timer_mtx);
        }
        
        Sleep(PROGRAM_SPEED);
        
        //try mutex lock on IO1
        mutexCheck = pthread_mutex_trylock(&io1_mtx);
        
        //if IO1 mutex is already locked
        if(mutexCheck != 0) {
            
            //Dequeue from IO1 waiting queue if possible
            if(size_is(io1_queue) > 0) {
                tempNode = dequeue(io1_queue);
                tempNode->data->currentState = ready;
                toStringFileVersion(tempNode->data, output_ptr);
                addPCB(PQ_p, tempNode->data);
                fprintf(output_ptr, "\nI/O 1 completion interrupt: PID %d is running, PID %d put in ready queue          <- IO1 COMPLETION\n",
                        runningPCB_p->pid, tempNode->data->pid);
            }
            //if not
            else {
                fprintf(output_ptr, "\nI/O 1 completion interrupt: PID %d is running, No avaliable PCB to put in ready queue          <- IO1 COMPLETION\n",
                        runningPCB_p->pid);
            }
            pthread_mutex_unlock(&io1_mtx);
        }
        else {
            pthread_mutex_unlock(&io1_mtx);
        }
        
        Sleep(PROGRAM_SPEED);
        
        //try mutex lock on IO2
        mutexCheck = pthread_mutex_trylock(&io2_mtx);
        
        //if IO2 mutex is already locked
        if(mutexCheck != 0) {
            
            //Dequeue from IO2 waiting queue if possible
            if(size_is(io2_queue) > 0) {
                tempNode = dequeue(io2_queue);
                tempNode->data->currentState = ready;
                toStringFileVersion(tempNode->data, output_ptr);
                addPCB(PQ_p, tempNode->data);
                fprintf(output_ptr, "\nI/O 2 completion interrupt: PID %d is running, PID %d put in ready queue          <- IO2 COMPLETION\n",
                        runningPCB_p->pid, tempNode->data->pid);
            }
            //if not
            else {
                fprintf(output_ptr, "\nI/O 2 completion interrupt: PID %d is running, No avaliable PCB to put in ready queue          <- IO2 COMPLETEION\n",
                        runningPCB_p->pid);
            }
            pthread_mutex_unlock(&io2_mtx);
        }
        else {
            pthread_mutex_unlock(&io2_mtx);
        }
        
        //Increment running PCB to simulate it running a single instruction
        fprintf(output_ptr, "\nCurrent PC: %d\n", runningPCB_p->pc++);
        
        //print array and queue sizes for testing
        fprintf(output_ptr, "\nPrint Data Structure Sizes for Testing:\n");
        fprintf(output_ptr, "The size of the array ADT is : %d\n"
                "The size of the I/O 1 queue : %d\n"
                "The size of the I/O 2 queue : %d\n\n", size_of(PQ_p),
                size_is(io1_queue), size_is(io2_queue)); 
        
        Sleep(PROGRAM_SPEED);
        
        //if pc val gets too its max, increment term count
        if(runningPCB_p->pc >= MAX_PC) {
            runningPCB_p->pc = 0;
            runningPCB_p->term_count++;
        }
        //Else check for IO interrupts
        else {

            fprintf(output_ptr, "Print IO traps Just for testing.\n");
            fprintf(output_ptr, "IO1: %d %d %d %d\n", runningPCB_p->IO_1_traps[0], runningPCB_p->IO_1_traps[1], runningPCB_p->IO_1_traps[2], runningPCB_p->IO_1_traps[3]);
            fprintf(output_ptr, "IO2: %d %d %d %d\n", runningPCB_p->IO_2_traps[0], runningPCB_p->IO_2_traps[1], runningPCB_p->IO_2_traps[2], runningPCB_p->IO_2_traps[3]);
            
            //notice for IDLE Process in running state only
            if(runningPCB_p->IO_1_traps[0] == -1) {
                fprintf(output_ptr, "(IDLE process has all Trap Simulators\nset to -1, since IDLE never calls IO TRAPS)\n");
            }
            
            // Check for I/O interrupts
            for(i = 0; i < 4; i++) {         
                if(runningPCB_p->pc == runningPCB_p->IO_1_traps[i]) {
                    temp = runningPCB_p;
                    temp->currentState = interrupted;
                    enqueue(io1_queue, temp);
                    runningPCB_p = search(PQ_p);
                    runningPCB_p->currentState = running;
                    fprintf(output_ptr, "\nI/O trap request: I/O device 1, PID %d put into waiting queue, PID %d dispatched          <- IO1 TRAP REQ.\n",
                            temp->pid, runningPCB_p->pid);
                }
                else if(runningPCB_p->pc == runningPCB_p->IO_2_traps[i]) {
                    temp = runningPCB_p;
                    temp->currentState = interrupted;
                    enqueue(io2_queue, temp);
                    runningPCB_p = search(PQ_p);
                    runningPCB_p->currentState = running;
                    fprintf(output_ptr, "\nI/O trap request: I/O device 2, PID %d put into waiting queue, PID %d dispatched          <- IO2 TRAP REQ.\n",
                            temp->pid, runningPCB_p->pid);
                }
                break;
            }
        }
    quantumNum++;
    }
    
    fprintf(output_ptr, "\n!!! SCROLL BACK TO TOP TO SEE SIMULATION PROGRESSION !!!\n\n");
    
    /* wait for the second thread to finish */
    
    if(pthread_join(timer_thread, NULL)) {

	fprintf(output_ptr, "Error 3131: joining timer thread\n");
	return 6;
    }

    if(pthread_join(io_1_thread, NULL)) {

	fprintf(output_ptr, "Error 6964747: joining io 1 thread\n");
	return 4;
    }

    if(pthread_join(io_2_thread, NULL)) {

	fprintf(output_ptr, "Error 11111: joining io 2 thread\n");
	return 5;
    }
    
    //Free Dynamically Allocated Data
    freePriority_queue(PQ_p);
    freeLinkedList(new_queue);
    freeLinkedList(io1_queue);
    freeLinkedList(io2_queue);
    freePCB(runningPCB_p);
    freePCB(temp);

    //Kills any running threads
    fprintf(output_ptr, "MAIN KILLING EVERYTHING\n");
    quantumNum = 0;
    fprintf(output_ptr, "Finished CPU thread...\n");
    
    return 0;
}