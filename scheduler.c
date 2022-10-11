/**
 *  scheduler.c
 *
 *  Full Name:
 *  Course section:
 *  Description of the program:
 *  
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "list.h"
#include "scheduler.h"
#include "process.h"

#define SIZE    100

struct node *head = NULL;

// get rounded up integer when dividing by 2
int getCeil(int x, int y) {
	return (x + (x % 2 == 0 ? 0 : 1)) / y;
}

// function to put node the the end of the linked list
void queueUp(struct node **headNode, Process *newProcess) {
	if(*headNode == NULL) {
		insert(&head, newProcess);
	} else {
		struct node *temp;
		struct node *prev;

		temp = *headNode;
		while(temp != NULL) {
			prev = temp;
			temp = temp->next;
		}
		struct node *newNode = malloc(sizeof(struct node));
		newNode->process = newProcess;
		newNode->next = NULL;

		prev->next = newNode;
	}
}

void queueShort(struct node **headNode, Process *newProcess, int *leftover) {
	if(*headNode == NULL) {
		insert(headNode, newProcess);
	} else {
		struct node *temp;
		struct node *prev;

		temp = *headNode;
		while(temp != NULL) {
			int pid = newProcess->pid;
			if(leftover[pid] < leftover[temp->process->pid]) {
				insert(headNode, newProcess);
				return;
			} else if(leftover[pid] == leftover[temp->process->pid]) {
				if(pid < temp->process->pid) {
					struct node *newNode = malloc(sizeof(struct node));
					newNode->process = newProcess;
					newNode->next = temp;

					prev->next = newNode;
					return;
				}
			}
			prev = temp;
			temp = temp->next;
		}
		struct node *newNode = malloc(sizeof(struct node));
		newNode->process = newProcess;
		newNode->next = NULL;

		prev->next = newNode;
	}
}

// struct node* findNode(struct node **headNode, Process *process) {
// 	struct node *foundNode = *headNode;
// 	while(foundNode != NULL) {
// 		if(foundNode->process->pid == process->pid) {
// 			return foundNode;
// 		}
// 		foundNode = foundNode->next;
// 	}
// 	return foundNode;
// }

// First come first serve function
void FCFS(Process *queue, int num_of_processes, char **filename, char *schedulerAlgo) {
	//initialize all required arrays and integers to keep track of states of each processes
	int done = 1;
	struct node *running = NULL;
	int time = 0;
	int *states = malloc(num_of_processes * sizeof(int));
	int *served = malloc(num_of_processes * sizeof(int));
	int *blocking = malloc(num_of_processes * sizeof(int));
	int *uptime = malloc(num_of_processes * sizeof(int));
	int countdown = 0;
	int cpu_time = 0;

	//Start the processes with default states which is 0
	for(int i=0; i<num_of_processes; i++) {
		states[i] = 0;
		served[i] = 0;
		blocking[i] = 0;
		uptime[i] = 0;
	}

	//While the algorithm is not done, compute the following
	while(done > 0) {
		printf("%d:", time);
		int cpu_use = 0;
		//For each of the processes, initialize the states to 1 which is ready and state 2 is blocked. The running state is only allowed for one process
		//Add this process to the queue
		for(int i = 0; i < num_of_processes; i++) {
			if(queue[i].arrival_time == time) {
				states[i] = 1;
				queueUp(&head, &queue[i]);
			}
			//If there are no running processes and the queue is not empty then make the running the head of the queue
			if(running == NULL && head != NULL) {
				running = head;
				states[running->process->pid]++;
				countdown = getCeil(running->process->cpu_time, 2);
				// printf("\n%d is the countdown\n", countdown);
			}
			//Check states for the process to see if it is not terminated
			if(states[i] > 0) {
				//Update the integers to keep track fo the uptime
				uptime[i]++;
				char state[8];
				if(running != NULL && running->process->pid == queue[i].pid) {
					strcpy(state, "running");
					cpu_use = 1;
				} else if(states[i] == 1) {
					strcpy(state, "ready");
				} else if(states[i] == 2) {
					strcpy(state, "blocked");
				}
				printf(" %d:%s", i, state);
			}
		}
		//Increment the cpu_time to update at the end.
		if(cpu_use > 0) {
			cpu_time++;
		}
		//Check if there are processes that are in blocking state, if they are then push them up the queue to run them and change state to ready
		for(int i = 0; i < num_of_processes; i++) {
			if(blocking[i] > 0) {
				blocking[i]--;
				if(blocking[i] == 0) {
					queueUp(&head, &queue[i]);
					states[i] = 1;
				}
			}
		}
		//Countdown is based on cpu_time so if it is not 0, then keep subracting it for it will allow us to terminate this process and move it into s ready
		//if the process has not been served, then move it blocked
		if(countdown > 0) {
			countdown--;
			if(countdown == 0) {
				if(served[running->process->pid] == 0) {
					served[running->process->pid]++;
					blocking[running->process->pid] = running->process->io_time;
					states[running->process->pid] = 2;
				} else {
					states[running->process->pid] = 0;
				}
				delete(&head, running->process);
				running = NULL;
			}
		}
		time++;
		printf("\n");
		
		//Check if the running list and linkedlist are null before we terminate the while loop
		if(running == NULL && head == NULL) {
			done = 0;
		}
		//If there are more processes in the blocked state, then keep the while loop running
		for(int i=0; i<num_of_processes; i++) {
			if(blocking[i] > 0) {
				done = 1;
			}
		}
	}

	//Print out the final finishing times
	//CPU utilization with 2 floting numbers
	printf("\nFinishing time: %d\n", time - 1);
	printf("CPU utilization: %.2f\n", (float) cpu_time / time);
	for(int i = 0; i < num_of_processes; i++) {
		printf("Turnaround process %d: %d\n", i, uptime[i]);
	}

	//Memory cleanup
	free(uptime);
	free(states);
	free(served);
	free(blocking);
}

// Round Robin scheduler function
void RR(Process *queue, int num_of_processes, char **filename, char *schedulerAlgo) {
	int done = 1;
	struct node *running = NULL;
	int time = 0;
	int *states = malloc(num_of_processes * sizeof(int));
	int *served = malloc(num_of_processes * sizeof(int));
	int *blocking = malloc(num_of_processes * sizeof(int));
	int *uptime = malloc(num_of_processes * sizeof(int));
	int *running_time = malloc(num_of_processes * sizeof(int));
	int countdown = 0;
	int cpu_time = 0;

	for(int i=0; i<num_of_processes; i++) {
		states[i] = 0;
		served[i] = 0;
		blocking[i] = 0;
		uptime[i] = 0;
		running_time[i] = getCeil(queue[i].cpu_time, 2);
	}

	while(done > 0) {
		printf("%d:", time);
		int cpu_use = 0;
		for(int i = 0; i < num_of_processes; i++) {
			if(queue[i].arrival_time == time) {
				states[i] = 1;
				queueUp(&head, &queue[i]);
			}
			if(running == NULL && head != NULL) {
				running = head;
				countdown = getCeil(running->process->cpu_time, 2);
				if(countdown > QUANTUM) {
					countdown = QUANTUM;
				}
				// printf("\n%d is the countdown\n", countdown);
			}
			if(states[i] > 0) {
				uptime[i]++;
				char state[8];
				if(running != NULL && running->process->pid == queue[i].pid) {
					strcpy(state, "running");
					cpu_use = 1;
				} else if(states[i] == 1) {
					strcpy(state, "ready");
				} else if(states[i] == 2) {
					strcpy(state, "blocked");
				}
				printf(" %d:%s", i, state);
			}
		}

		if(cpu_use > 0) {
			cpu_time++;
		}

		for(int i = 0; i < num_of_processes; i++) {
			if(blocking[i] > 0) {
				blocking[i]--;
				if(blocking[i] == 0) {
					queueUp(&head, &queue[i]);
					states[i] = 1;
				}
			}
		}
		
		if(countdown > 0) {
			int pid = running->process->pid;
			countdown--;
			running_time[pid]--;
			if(countdown == 0 || running_time[pid] == 0) {
				if(served[pid] == 0) {
					if(running_time[pid] == 0) {	
						served[pid]++;
						blocking[pid] = running->process->io_time;
						states[pid] = 2;
						running_time[pid] = getCeil(running->process->cpu_time, 2);
					} else {
						queueUp(&head, &queue[pid]);
					}
				} else if(served[pid] > 0) {
					if(running_time[pid] > 0) {
						queueUp(&head, &queue[pid]);
					} else {
						states[pid] = 0;
					}
				}
				countdown = 0;
				delete(&head, running->process);
				running = NULL;
			}
		}
		time++;
		printf("\n");
		
		if(running == NULL && head == NULL) {
			done = 0;
		}
		for(int i=0; i<num_of_processes; i++) {
			if(blocking[i] > 0 || states[i] > 0) {
				done = 1;
			}
		}
		if(time > 30) {
			done = 0;
		}
	}

	printf("\nFinishing time: %d\n", time - 1);
	printf("CPU utilization: %.2f\n", (float) cpu_time / time);
	for(int i = 0; i < num_of_processes; i++) {
		printf("Turnaround process %d: %d\n", i, uptime[i]);
	}

	free(running_time);
	free(uptime);
	free(states);
	free(served);
	free(blocking);
}

// Shortest Job First scheduler function
void SJF(Process *queue, int num_of_processes) {
	int done = 1;
	struct node *running = NULL;
	int time = 0;
	int *states = malloc(num_of_processes * sizeof(int));
	int *served = malloc(num_of_processes * sizeof(int));
	int *blocking = malloc(num_of_processes * sizeof(int));
	int *uptime = malloc(num_of_processes * sizeof(int));
	int *leftover = malloc(num_of_processes * sizeof(int));
	int countdown = 0;
	int cpu_time = 0;

	for(int i = 0; i < num_of_processes; i++) {
		states[i] = 0;
		served[i] = 0;
		blocking[i] = 0;
		uptime[i] = 0;
		leftover[i] = queue[i].cpu_time + queue[i].io_time;
	}

	while(done > 0) {
		printf("%d:", time);
		int cpu_use = 0;

		for(int i = 0; i < num_of_processes; i++) {
			if(queue[i].arrival_time == time) {
				states[i] = 1;
				queueShort(&head, &queue[i], leftover);
			}
			if(running == NULL && head != NULL) {
				running = head;
				countdown = getCeil(running->process->cpu_time, 2);
			}
			if(states[i] > 0) {
				uptime[i]++;
				char state[8];
				if(running != NULL && running->process->pid == queue[i].pid) {
					strcpy(state, "running");
					cpu_use = 1;
				} else if(states[i] == 1) {
					strcpy(state, "ready");
				} else if(states[i] == 2) {
					strcpy(state, "blocked");
				}
				printf(" %d:%s", i, state);
			}
		}

		if(cpu_use > 0) {
			cpu_time++;
		}

		for(int i = 0; i < num_of_processes; i++) {
			if(blocking[i] > 0) {
				blocking[i]--;
				leftover[i]--;
				if(blocking[i] == 0) {
					queueShort(&head, &queue[i], leftover);
					states[i] = 1;
				}
			}
		}

		if(countdown > 0) {
			int pid = running->process->pid;
			countdown--;
			leftover[pid]--;
			if(countdown == 0) {
				if(served[pid] == 0) {
					served[pid]++;
					blocking[pid] = running->process->io_time;
					states[pid] = 2;
				} else {
					states[pid] = 0;
				}
				delete(&head, running->process);
				running = NULL;
			}
		}

		time++;
		printf("\n");

		if(running == NULL && head == NULL) {
			done = 0;
		}

		for(int i = 0; i < num_of_processes; i++) {
			if(blocking[i] > 0 || states[i] > 0) {
				done = 1;
			}
		}
		if(time > 30) {
			done = 0;
		}
	}

	printf("\nFinishing time: %d\n", time - 1);
	printf("CPU utilization: %.2f\n", (float) cpu_time / time);
	for(int i = 0; i < num_of_processes; i++) {
		printf("Turnaround process %d: %d\n", i, uptime[i]);
	}

	free(states);
	free(served);
	free(blocking);
	free(uptime);
	free(leftover);
}

int main(int argc, char *argv[])
{
	FILE *fp;

	int num_of_processes;

	// get the size of the filename minus the extension, ".txt"
	size_t filenameSize = strlen(argv[1]) - strlen(".txt");
	// weird quirk of code where char filename[x], where x is a constant int, doesn't work with copying the filename but malloc works
	char *filename = malloc(sizeof(char) * filenameSize + 1);
	strncpy(filename, argv[1], filenameSize);
	char schedulerAlgo[1];
	strcpy(schedulerAlgo, argv[2]);
	// printf("file is %s while argv[1] is %s and scheduler algorithm is %s\nTesting getCeil: %d\n", filename, argv[1], schedulerAlgo, getCeil(7, 2));
	
	fp  = fopen(argv[1],"r");
	fscanf(fp, "%d", &num_of_processes);    
	if (num_of_processes > 0){
		Process *queue = malloc(num_of_processes * sizeof(Process));      

		for (int i=0; i<num_of_processes; i++){
			fscanf(fp, "%d %d %d %d",
					&queue[i].pid, 
					&queue[i].cpu_time, 
					&queue[i].io_time, 
					&queue[i].arrival_time); 
		}                                                                               

		// for(int i=0; i<num_of_processes; i++) {
		// 	printf("A: %d B: %d  C: %d D: %d \n",
		// 			queue[i].pid, 
		// 			queue[i].cpu_time, 
		// 			queue[i].io_time, 
		// 			queue[i].arrival_time);    
		// }
		char *output_name = malloc(sizeof(char) * (strlen(argv[1]) + strlen(argv[2]) + 1));
		strncpy(output_name, argv[1], filenameSize);
		strcat(output_name, "-");
		strcat(output_name, schedulerAlgo);
		strcat(output_name, ".txt");
		// printf("%s\n", output_name);
		// redirect stdout to file
		// freopen(output_name, "a+", stdout);
		switch(argv[2][0]) {
			case '0':
				FCFS(queue, num_of_processes, &filename, schedulerAlgo);
				break;
			case '1':
				RR(queue, num_of_processes, &filename, schedulerAlgo);
				break;
			case '2':
				SJF(queue, num_of_processes);
				break;
		}
		
		free(output_name);
		free(filename);
		free(queue); 
	}

	fclose(fp);


	return 0;
}
