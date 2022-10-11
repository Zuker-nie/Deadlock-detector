/**
 *  process.h
 *
 *  Full Name: Zuker Nie Chen
 *  Course section: M
 *  Representation of a process in the system.
 *  
 */

#ifndef PROCESS_H
#define PROCESS_H

#include "list.h"

// representation of a a process
typedef struct process {
    struct node *activities;
    int totalRuntime; //accounts of all cycles
    int cycleTime; //actual use of cpu cycles
    int aborted;
    int *resourceAllocated;
    int blocked;
    int skip;
    int safe;
    int *claim;
} Process;

#endif