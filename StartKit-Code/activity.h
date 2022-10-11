/**
 *	activity.h
 * 
 * 	Full Name: Zuker Nie Chen
 *  Course section: M
 *  Description of the program: FIFO And Banker's Algorithm to simulate a run-time of processes
 * 
 */

#ifndef ACTIVITY_H
#define ACTIVITY_H

typedef struct activity {
	char name[12];
	int pid;
	int val2;
	int val3;
} Activity;

#endif
// initiate
// compute
// request
// release
// termininate