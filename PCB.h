#ifndef PCB_H
#define PCB_H
#define MAX_PC 2345

#include <time.h>

/*
Group 5: Nok Him Nam, Jowy Tran, Mark Johnson, David Humphreys
Date : 28/ 01/ 2015
Assignment : Problem 2 - Scheduling
*/

enum state_type {
	created,
	ready,
	running,
	blocked,
	interrupted,
	terminated
};

typedef enum state_type State;

struct PCB {
	char name[20];
	int pid;
	int priority;
	int interrupt_simulator;
	unsigned int pc;
	State currentState;

	// new stuff
        int max_pc; //jowy added
	char* creation; //jowy renamed to match diagram
	char* termination; //jowy renamed to match diagram
	int terminate;
	int term_count; //jowy renamed to match diagram
	int IO_1_traps[4]; //jowy renamed to match diagram
	int IO_2_traps[4]; //jowy renamed to match diagram
};

typedef struct PCB* PCB_p;

PCB_p createPCB(char* name, int newPid, int newPriority, int theInterruptSimulator,
	int thePcValue, State theState);
PCB_p createIdlePCB();
char* getName(PCB_p pcb);
int getPid(PCB_p pcb);
int getPriority(PCB_p pcb);
int getInterruptSimulator(PCB_p pcb);
int getPCValue(PCB_p pcb);
char* getState(PCB_p pcb);
int getMax_PC(PCB_p pcb); //jowy added

void freePCB(PCB_p pcb);
void toString(PCB_p pcb);
void toStringFileVersion(PCB_p pcb, FILE* theOuputFile);

void setName(PCB_p pcb, char* newName);
void setPid(PCB_p pcb, int newPid);
void setPriority(PCB_p pcb, int newPriority);
void setInterruptSimulator(PCB_p pcb, int theInterruptSimulator);
void setPCValue(PCB_p pcb, int thePCValue);
void setTheState(PCB_p pcb, State theState);

// New Stuff
void generateArray(int arrayOne[4], int arrayTwo[4]);
void printArray(int theArray[4]);
char* getTheCurrentTime(void);


#endif
