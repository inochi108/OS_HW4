#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <time.h>
#include "PCB.h"

/*
 ===========================================================================
 Name        : 422_HW3.c
 Date        : 2/12/2016
 Authors     : Nok Him Nam, Jowy Tran, Mark Johnson, David Humphreys
 Description : Discontinuities with I/O service request traps and interrupts
 ===========================================================================
 */
// returns the pointer to the list; NULL if list not created
PCB_p createPCB(char* name, int newPid, int newPriority, int theInterruptSimulator,
	int thePcValue, State theState) {
	if (newPriority > 15 || newPriority < 0) {
		printf("Error 1212123: Priority must be 0 - 15. PCD not Created.");
	}
	//struct control_block_type testing;
	PCB_p pcb = malloc(sizeof(struct PCB));
	// if allocation was successful
	if (pcb != NULL) {

            strcpy(pcb->name, name);
            pcb->pid = newPid;
            pcb->priority = newPriority;
            pcb->interrupt_simulator = theInterruptSimulator;
            pcb->pc = thePcValue;
            pcb->currentState = theState;

            //New stuff
            //pcb->max_pc = ?   jowy added  (do we need a new parameter?)
            pcb->creation = getTheCurrentTime();
            pcb->terminate = (rand() % 11) +1;
            pcb->term_count = 0;
            generateArray(pcb->IO_1_traps, pcb->IO_2_traps);

	}
	return pcb;
}
//Creates an IDLE PCB
// returns the pointer to the list; NULL if list not created
PCB_p createIdlePCB() {
        int i = 0;
	//struct control_block_type testing;
	PCB_p pcb = createPCB("IDLE_PCB", 0, 0, 0, 0, running);
	// if allocation was successful

        strcpy(pcb->name, "IDLE_PCB");
        pcb->pid = -1;
        pcb->priority = 15;
        pcb->interrupt_simulator = 0;
        pcb->pc = 0;
        pcb->currentState = running;

            //New stuff
            //pcb->max_pc = ?   jowy added  (do we need a new parameter?)
        pcb->creation = getTheCurrentTime();
        pcb->terminate = INT_MAX;
        pcb->term_count = 0;
        for (; i < 4; i++) {
            pcb->IO_1_traps[i] = -1;
            pcb->IO_2_traps[i] = -1;
        }
	return pcb;
}

char* getName(PCB_p pcb) {
	return pcb->name;
}

int getPid(PCB_p pcb) {
	return pcb->pid;
}

int getPriority(PCB_p pcb) {
	return pcb->priority;
}

int getInterruptSimulator(PCB_p pcb) {
	return pcb->interrupt_simulator;
}

int getPCValue(PCB_p pcb) {
	return pcb->pc;
}

char* getState(PCB_p pcb) {

	switch (pcb->currentState) {
	case created: return "created"; break;
	case ready: return "ready"; break;
	case running: return "running"; break;
	case blocked: return "blocked"; break;
	case interrupted: return "interrupted"; break;
	case terminated: return "terminated"; break;
	default: return "ERROR";
	}
}

int getMax_PC(PCB_p pcb) { //jowy added
    return pcb->max_pc;
}

void setName(PCB_p pcb, char* newName) {
	strcpy(pcb->name, newName);
}

void setPid(PCB_p pcb, int newPid) {
	pcb->pid = newPid;
}

void setPriority(PCB_p pcb, int newPriority) {
	pcb->priority = newPriority;
}

void setInterruptSimulator(PCB_p pcb, int theInterruptSimulator) {
	pcb->interrupt_simulator = theInterruptSimulator;
}

void setPCValue(PCB_p pcb, int thePCValue) {
	pcb->pc = thePCValue;
}

void setTheState(PCB_p pcb, State theState) {
	pcb->currentState = theState;
}

void setMax_PC(PCB_p pcb, int theMax_PC) { //jowy added
    pcb->max_pc = theMax_PC;
}

void freePCB(PCB_p pcb) {
	free(pcb);
	pcb = NULL;
}

void toString(PCB_p pcb) {
        //note: %u is official print reference for unsigned integer.
	printf("Name: %s, terminate: %d, Priority: %d\nterm_count : %d, PC : %u, State : %s\n\n",
		pcb->name, pcb->terminate, pcb->priority,
		pcb->term_count, pcb->pc, getState(pcb));
}

void toStringFileVersion(PCB_p pcb, FILE* theOuputFile) {
	fprintf(theOuputFile, "Name: %s, terminate: %d, Priority: %d\nterm_count : %d, PC : %u, State : %s\n\n",
		pcb->name, pcb->terminate, pcb->priority,
		pcb->term_count, pcb->pc, getState(pcb));
}

void generateArray(int arrayOne[4], int arrayTwo[4]) {

	int i = 0;
	int spaceNum = MAX_PC / 8;

	for (i = 0; i < 8; i++) {

		if (i % 2 == 0) {
			arrayOne[i/2] = i * spaceNum + rand() % spaceNum;
		}
		else {
			arrayTwo[i/2] = i * spaceNum + rand() % spaceNum;
		}
	}
}

void printArray(int theArray[4]) {

	int i = 0;

	for (i = 0; i < 4; i++) {
		printf("%d ", theArray[i]);
	}

	printf("\n");
}

char* getTheCurrentTime(void) {

	time_t currentTimeAccesser;
	char* tempString;
	currentTimeAccesser = time(NULL);
	tempString = ctime(&currentTimeAccesser);
	return tempString;
}