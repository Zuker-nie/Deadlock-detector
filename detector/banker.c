/**
 *  banker.c
 *
 *  Full Name: Zuker Nie Chen
 *  Course section: M
 *  Description of the program: FIFO And Banker's Algorithm to simulate a run-time of processes
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "list.h"
#include "banker.h"
#include "process.h"
#include "activity.h"


#define SIZE    100

void FIFO(Process *queue, int num_of_tasks, int *resource_type_amount, int num_of_resource_types)
{
	//set the attribute of each task to this cycle time when it "terminate"
	int tasksToRun = num_of_tasks;

	//instantiate a queue of blocked requests
	int blockedQueue[num_of_tasks];
	int blockedQueueIndex = 0;
	
	//reset the lists of blocked items
	for(int i = 0; i < num_of_tasks; i++)
	{
		blockedQueue[i] = 0;
	}

	//this will iterate through all cycles available for the tasks
	while(tasksToRun > 0)
	{
		//checking for blocked
		for(int i = 0; i < blockedQueueIndex; i++)
		{
			queue[blockedQueue[i]].totalRuntime++;
			//handle blocked state tasks
			//compare resource requested with the resource amount OF THAT TYPE.
			if(queue[blockedQueue[i]].activities->activity->val3 <= resource_type_amount[queue[blockedQueue[i]].activities->activity->val2-1])
			{
				// printing the pid which starts at 1 but index is at 0
				// printf("task %d\n", blockedQueue[i]+1);
				// printf("requested %d\n", queue[blockedQueue[i]].activities->activity->val3);
				// printf("avail %d\n\n", resource_type_amount[queue[blockedQueue[i]].activities->activity->val2-1]);
				//let the task go back and request again

				//add to allocated resource for this task
				queue[blockedQueue[i]].resourceAllocated += queue[blockedQueue[i]].activities->activity->val3;

				//subract from resources avail
				resource_type_amount[queue[blockedQueue[i]].activities->activity->val2-1] -= queue[blockedQueue[i]].activities->activity->val3;

				//point to next activity of this task
				queue[blockedQueue[i]].activities = queue[blockedQueue[i]].activities->next;

				queue[blockedQueue[i]].cycleTime++;
				

				queue[blockedQueue[i]].blocked = 0;
				queue[blockedQueue[i]].skip = 1;

				//reset the queue of blocked items
				for(int j = i; j < blockedQueueIndex + 1; j++)
				{
					int temp = blockedQueue[j];
					blockedQueue[j] = blockedQueue[j+1];
					blockedQueue[j+1] = temp;
				}
				//remove the number of blocked items
				blockedQueueIndex--;
				//counter act the moving of the blocked queue
				i--;
			}
		}


		//keep track of resources available for the current cycle
		int currentCycleRes[num_of_resource_types];
		for(int i = 0; i < num_of_resource_types; i++)
		{
			currentCycleRes[i] = 0;
		}

		//iterate through each task to start looking at their activity
		for(int i = 0; i < num_of_tasks; i++)
		{
			//to skip over finished task because we made it to point to a NULL node
			//Done || aborted || blocked tasks || skip tasks
			if(queue[i].activities == NULL || queue[i].aborted == 1 || queue[i].blocked == 1 || queue[i].skip == 1)
			{
				continue;
			}
			//switch case for each string
			if(strcmp(queue[i].activities->activity->name,"initiate") == 0)
			{
				// printf("init\n");
				// printf("%d ", queue[i].activities->activity->pid);
				// printf("%d ", queue[i].activities->activity->val2);
				// printf("%d\n", queue[i].activities->activity->val3);
				queue[i].totalRuntime++;
				queue[i].cycleTime++;
				queue[i].activities = queue[i].activities->next; //move to next activity
			}
			else if(strcmp(queue[i].activities->activity->name,"compute") == 0)
			{
				// printf("compute\n");
				// printf("%d ", queue[i].activities->activity->pid);
				// printf("%d ", queue[i].activities->activity->val2);
				// printf("%d\n", queue[i].activities->activity->val3);
				queue[i].totalRuntime++;
				queue[i].cycleTime++;
				queue[i].activities->activity->val2--;
				//only move when it is done computing aka val2 = 0
				if(queue[i].activities->activity->val2 == 0)
				{
					queue[i].activities = queue[i].activities->next; //move to next activity
				}
			}
			else if(strcmp(queue[i].activities->activity->name,"request") == 0)
			{
				// printf("request\n");
				// printf("%d ", queue[i].activities->activity->pid);
				// printf("%d ", queue[i].activities->activity->val2);
				// printf("%d\n", queue[i].activities->activity->val3);

				//update total runtime regardless of if it runs or not
				queue[i].totalRuntime++;
				//current cycle resources has to be >= to the amount requested
				if(resource_type_amount[queue[i].activities->activity->val2-1] >= queue[i].activities->activity->val3)
				{
					//add val3 (amount wanted) to the allocated resources of type val2
					queue[i].resourceAllocated[queue[i].activities->activity->val2-1] += queue[i].activities->activity->val3;
					queue[i].cycleTime++;
					resource_type_amount[queue[i].activities->activity->val2-1] -= queue[i].activities->activity->val3;
					queue[i].activities = queue[i].activities->next;
					// printf("%d\n\n", queue[i].resourceAllocated[queue[i].activities->activity->val2-1]);
				}
				else
				{
					//insert the process into the blocked queue if it is not able to request
					blockedQueue[blockedQueueIndex] = i;
					blockedQueueIndex++;
					queue[i].blocked = 1;
					// printf("blocked\n\n");
				}
			}
			else if(strcmp(queue[i].activities->activity->name,"release") == 0)
			{
				// printf("release\n");
				// printf("%d ", queue[i].activities->activity->pid);
				// printf("%d ", queue[i].activities->activity->val2);
				// printf("%d\n", queue[i].activities->activity->val3);
				queue[i].totalRuntime++;
				queue[i].cycleTime++;
				currentCycleRes[queue[i].activities->activity->val2-1] += queue[i].activities->activity->val3; //add the num of resources released
				queue[i].resourceAllocated -= queue[i].activities->activity->val3;
				queue[i].activities = queue[i].activities->next;
			}
			else if(strcmp(queue[i].activities->activity->name,"terminate") == 0)
			{
				// printf("terminate\n");
				// printf("%d ", queue[i].activities->activity->pid);
				// printf("%d ", queue[i].activities->activity->val2);
				// printf("%d\n", queue[i].activities->activity->val3);
				tasksToRun--;
				queue[i].activities = queue[i].activities->next; //point to null node
			}
			
		}

		//update after it releases
		for(int i = 0; i < num_of_resource_types; i++)
		{
			resource_type_amount[i] += currentCycleRes[i];
		}
		
		//checking for deadlocking items
		for(int i = 0; i < num_of_tasks; i++)
		{
			if(blockedQueueIndex == tasksToRun)
			{
				if(queue[i].blocked == 1)
				{
					//check if resources requested can be granted already, if yes then break. Since a task can "run" then no more deadlock
					if(queue[i].activities->activity->val3 <= resource_type_amount[queue[i].activities->activity->val2-1])
					{
						break;
					}
					//deadlock it
					// printf(" task %d\n", i + 1);
					// printf(" amount %d\n", queue[i].activities->activity->val3);
					// printf(" type %d\n", queue[i].activities->activity->val2);
					for(int j = 0; j < num_of_resource_types; j++)
					{
						if(queue[i].resourceAllocated[j] > 0)
						{
							// printf("\n before release type amount and res allo %d %d\n", resource_type_amount[j], queue[i].resourceAllocated[j]);
							resource_type_amount[j] += queue[i].resourceAllocated[j];
							queue[i].resourceAllocated[j] = 0;
							// printf("\n after release type amount and res allo %d %d\n", resource_type_amount[j], queue[i].resourceAllocated[j]);
						}
					}
					for(int k = 0; k < blockedQueueIndex; k++)
					{
						if(blockedQueue[k] == i)
						{
							for(int l = k; l < blockedQueueIndex - 1; l++)
							{
								int temp = blockedQueue[l];
								blockedQueue[l] = blockedQueue[l+1];
								blockedQueue[l+1] = temp;
							}
							break;
						}
					}
					queue[i].aborted = 1;
					queue[i].blocked = 0;
					blockedQueueIndex--;
					tasksToRun--;
				}
			}
			//reset the skipping flag
			queue[i].skip = 0;
		}
	}
}

void Banker(Process *queue, int num_of_tasks, int *resource_type_amount, int num_of_resource_types)
{
	//set the attribute of each task to this cycle time when it "terminate"
	int tasksToRun = num_of_tasks;

	//instantiate a queue of blocked requests
	int blockedQueue[num_of_tasks];
	int blockedQueueIndex = 0;
	
	//reset the lists of blocked items
	for(int i = 0; i < num_of_tasks; i++)
	{
		blockedQueue[i] = 0;
	}

	//int maxrun = 0;

	//this will iterate through all cycles available for the tasks
	while(tasksToRun > 0)
	{
		//printf("cycle: %d\n", maxrun);
		//maxrun++;
		//checking for blocked
		for(int i = 0; i < blockedQueueIndex; i++)
		{
			queue[blockedQueue[i]].totalRuntime++;
			//handle blocked state tasks
			
			if(queue[blockedQueue[i]].claim[queue[blockedQueue[i]].activities->activity->val2-1] - queue[blockedQueue[i]].resourceAllocated[queue[blockedQueue[i]].activities->activity->val2-1] <= resource_type_amount[queue[blockedQueue[i]].activities->activity->val2-1])
			{
				//compare resource requested with the resource amount OF THAT TYPE.
				if(queue[blockedQueue[i]].activities->activity->val3 <= resource_type_amount[queue[blockedQueue[i]].activities->activity->val2-1])
				{
					//printing the pid which starts at 1 but index is at 0
					//printf("task %d\n", blockedQueue[i]+1);
					//printf("requested %d\n", queue[blockedQueue[i]].activities->activity->val3);
					//printf("avail %d\n\n", resource_type_amount[queue[blockedQueue[i]].activities->activity->val2-1]);
					//let the task go back and request again

					//add to allocated resource for this task
					queue[blockedQueue[i]].resourceAllocated[queue[blockedQueue[i]].activities->activity->val2-1] += queue[blockedQueue[i]].activities->activity->val3;

					//subract from resources avail
					resource_type_amount[queue[blockedQueue[i]].activities->activity->val2-1] -= queue[blockedQueue[i]].activities->activity->val3;

					//point to next activity of this task
					queue[blockedQueue[i]].activities = queue[blockedQueue[i]].activities->next;

					queue[blockedQueue[i]].cycleTime++;
					

					queue[blockedQueue[i]].blocked = 0;
					queue[blockedQueue[i]].skip = 1;

					//reset the queue of blocked items
					for(int j = i; j < blockedQueueIndex + 1; j++)
					{
						int temp = blockedQueue[j];
						blockedQueue[j] = blockedQueue[j+1];
						blockedQueue[j+1] = temp;
					}
					//remove the number of blocked items
					blockedQueueIndex--;
					//counter act the moving of the blocked queue
					i--;
				}
			}
			
		}

		//printf("quit\n");

		//keep track of resources available for the current cycle
		int currentCycleRes[num_of_resource_types];
		for(int i = 0; i < num_of_resource_types; i++)
		{
			currentCycleRes[i] = 0;
		}

		//iterate through each task to start looking at their activity
		for(int i = 0; i < num_of_tasks; i++)
		{
			//to skip over finished task because we made it to point to a NULL node
			//Done || aborted || blocked tasks || skip tasks
			if(queue[i].activities == NULL || queue[i].aborted == 1 || queue[i].blocked == 1 || queue[i].skip == 1)
			{
				continue;
			}
			//switch case for each string
			if(strcmp(queue[i].activities->activity->name,"initiate") == 0)
			{
				// printf("init\n");
				// printf("%d ", queue[i].activities->activity->pid);
				// printf("%d ", queue[i].activities->activity->val2);
				// printf("%d\n", queue[i].activities->activity->val3);
				
				//abort instantly if claim is more than available
				if(resource_type_amount[queue[i].activities->activity->val2-1] < queue[i].activities->activity->val3)
				{
					//abort the task
					queue[i].aborted = 1;
					//message
					printf("  Banker aborts task %d before run begins:\n       claim for resource %d (%d) exceeds number of units present (%d)\n",
					i+1, queue[i].activities->activity->val2, 
					queue[i].activities->activity->val3,
					resource_type_amount[queue[i].activities->activity->val2-1]);
					tasksToRun--;
					continue;
				}
				queue[i].totalRuntime++;
				queue[i].cycleTime++;
				queue[i].claim[queue[i].activities->activity->val2-1] = queue[i].activities->activity->val3;
				queue[i].activities = queue[i].activities->next; //move to next activity
			}
			else if(strcmp(queue[i].activities->activity->name,"compute") == 0)
			{
				// printf("compute\n");
				// printf("%d ", queue[i].activities->activity->pid);
				// printf("%d ", queue[i].activities->activity->val2);
				// printf("%d\n", queue[i].activities->activity->val3);
				queue[i].totalRuntime++;
				queue[i].cycleTime++;
				queue[i].activities->activity->val2--;
				//only move when it is done computing aka val2 = 0
				if(queue[i].activities->activity->val2 == 0)
				{
					queue[i].activities = queue[i].activities->next; //move to next activity
				}
			}
			else if(strcmp(queue[i].activities->activity->name,"request") == 0)
			{
				// printf("request\n");
				// printf("%d ", queue[i].activities->activity->pid);
				// printf("%d ", queue[i].activities->activity->val2);
				// printf("%d\n", queue[i].activities->activity->val3);
				// printf("initial claim: %d res allocated: %d res avail: %d\n",
				// queue[i].claim[queue[i].activities->activity->val2-1], 
				// queue[i].resourceAllocated[queue[i].activities->activity->val2-1],
				// resource_type_amount[queue[i].activities->activity->val2-1]);

				//update total runtime regardless of if it runs or not
				queue[i].totalRuntime++;
				//can only request if it is in safe state mode
				//if requested amount is > than claim amount, then change to not safe
				//else it can request of that amount for that type of resource
				if(queue[i].claim[queue[i].activities->activity->val2-1] < queue[i].activities->activity->val3)
				{
					queue[i].aborted = 1;
					//release all the current resources
					for(int j = 0; j < num_of_resource_types; j++)
					{
						if(queue[i].resourceAllocated[j] > 0)
						{
							// printf("\n before release type amount and res allo %d %d\n", resource_type_amount[j], queue[i].resourceAllocated[j]);
							resource_type_amount[j] += queue[i].resourceAllocated[j];
							queue[i].resourceAllocated[j] = 0;
							// printf("\n after release type amount and res allo %d %d\n", resource_type_amount[j], queue[i].resourceAllocated[j]);
						}
					}
					//message when aborted
					printf("  Banker aborts task %d:\n       request for resource %d (%d) exceeds number of units claimed (%d)\n",
					i+1, queue[i].activities->activity->val2, 
					queue[i].activities->activity->val3, 
					queue[i].claim[queue[i].activities->activity->val2-1]);
					tasksToRun--;
					continue;
				}

				//current cycle resources has to be >= to the amount requested
				//[initial claim] - [currently allocated units for this task of this type] <= [current # of units in requested type]
				//check for safe state
				if(queue[i].claim[queue[i].activities->activity->val2-1] - queue[i].resourceAllocated[queue[i].activities->activity->val2-1] <= resource_type_amount[queue[i].activities->activity->val2-1])
				{
					if(resource_type_amount[queue[i].activities->activity->val2-1] > 0 && resource_type_amount[queue[i].activities->activity->val2-1] >= queue[i].activities->activity->val3)
					{
						//add val3 (amount wanted) to the allocated resources of type val2
						queue[i].resourceAllocated[queue[i].activities->activity->val2-1] += queue[i].activities->activity->val3;
						queue[i].cycleTime++;
						resource_type_amount[queue[i].activities->activity->val2-1] -= queue[i].activities->activity->val3;
						queue[i].activities = queue[i].activities->next;
						//printf("%d\n\n", queue[i].resourceAllocated[queue[i].activities->activity->val2-1]);
					}
					else
					{
						//insert the process into the blocked queue if it is not able to request
						blockedQueue[blockedQueueIndex] = i;
						blockedQueueIndex++;
						queue[i].blocked = 1;
						//printf("blocked 1\n\n");
					}
				}
				else
				{
					//insert the process into the blocked queue if it is not able to request
					blockedQueue[blockedQueueIndex] = i;
					blockedQueueIndex++;
					queue[i].blocked = 1;
					//printf("blocked 2\n\n");
				}
				
			}
			else if(strcmp(queue[i].activities->activity->name,"release") == 0)
			{
				// printf("release\n");
				// printf("%d ", queue[i].activities->activity->pid);
				// printf("%d ", queue[i].activities->activity->val2);
				// printf("%d\n", queue[i].activities->activity->val3);
				queue[i].totalRuntime++;
				queue[i].cycleTime++;
				currentCycleRes[queue[i].activities->activity->val2-1] += queue[i].activities->activity->val3; //add the num of resources released
				queue[i].resourceAllocated[queue[i].activities->activity->val2-1] -= queue[i].activities->activity->val3; //release the resources
				queue[i].activities = queue[i].activities->next;
			}
			else if(strcmp(queue[i].activities->activity->name,"terminate") == 0)
			{
				// printf("terminate\n");
				// printf("%d ", queue[i].activities->activity->pid);
				// printf("%d ", queue[i].activities->activity->val2);
				// printf("%d\n", queue[i].activities->activity->val3);
				tasksToRun--;
				queue[i].activities = queue[i].activities->next; //point to null node
			}
		}

		//update after it releases
		for(int i = 0; i < num_of_resource_types; i++)
		{
			resource_type_amount[i] += currentCycleRes[i];
		}
		//reset skip flag
		for(int i = 0; i < num_of_tasks; i++)
		{
			queue[i].skip = 0;
		}
	}
}

int main(int argc, char *argv[])
{
	FILE *fp;

	int num_of_tasks;
	int num_of_resource_types;
	//int resource_type_amount;
	
	//FIFO
	fp  = fopen(argv[1],"r");
	fscanf(fp, "%d %d", &num_of_tasks, &num_of_resource_types);

	//initialize the number of resources avail per type of resources
	int resourcePerType[num_of_resource_types];
	for(int i = 0; i < num_of_resource_types; i++)
	{
		fscanf(fp, "%d", &resourcePerType[i]);
	}

	if (num_of_tasks > 0)
	{
		Process *queue = malloc(num_of_tasks * sizeof(Process));
		for (int i=0; i<num_of_tasks; i++){
			
			queue[i].resourceAllocated = malloc(num_of_resource_types * sizeof(int));

			struct node *tail = queue[i].activities;
			while(1) {
				Activity *newActivity = malloc(sizeof(Activity));
				fscanf(fp, "%s %d %d %d",
					newActivity->name,
					&newActivity->pid,
					&newActivity->val2,
					&newActivity->val3);
				if(tail) {
					insertTail(&tail, newActivity);
				} else {
					insert(&queue[i].activities, newActivity);
					tail = queue[i].activities;
				}
				if(strcmp(newActivity->name, "terminate") == 0) {
					break;
				}
			}
		}
		fclose(fp);

		FIFO(queue, num_of_tasks, resourcePerType, num_of_resource_types);
		

		//BANKERS
		fp  = fopen(argv[1],"r");
		fscanf(fp, "%d %d", &num_of_tasks, &num_of_resource_types);
		//initialize the number of resources avail per type of resources
		int resourcePerTypeB[num_of_resource_types];
		for(int i = 0; i < num_of_resource_types; i++)
		{
			fscanf(fp, "%d", &resourcePerTypeB[i]);
		}
		Process *queueB = malloc(num_of_tasks * sizeof(Process));
		for (int i=0; i<num_of_tasks; i++){
			queueB[i].resourceAllocated = malloc(num_of_resource_types * sizeof(int));
			struct node *tail = queueB[i].activities;
			while(1) {
				Activity *newActivity = malloc(sizeof(Activity));
				fscanf(fp, "%s %d %d %d",
					newActivity->name,
					&newActivity->pid,
					&newActivity->val2,
					&newActivity->val3);
				if(tail) {
					insertTail(&tail, newActivity);
				} else {
					insert(&queueB[i].activities, newActivity);
					tail = queueB[i].activities;
				}
				if(strcmp(newActivity->name, "terminate") == 0) {
					break;
				}
			}
			//initialize and allocate the claim of each task
			queueB[i].claim = malloc(num_of_resource_types * sizeof(int)); 
			for(int j = 0; j < num_of_resource_types; j++)
			{
				queueB[i].claim[j] = 0;
				queueB[i].resourceAllocated[j] = 0;
			}
		}
		//tester
		/* for(int i = 0; i < num_of_tasks; i++) {
			struct node *temp = queueB[i].activities;
			while(temp) {
				Activity *a = temp->activity;
				printf("%s %d %d %d\n", a->name, a->pid, a->val2, a->val3);
				temp = temp->next;
			}
		} */
		fclose(fp);
		Banker(queueB, num_of_tasks, resourcePerTypeB, num_of_resource_types);


		//PRINTING
		int runtimeOfAlgo = 0;
		int runtimeOfAlgoB = 0;
		double unusedCycles = 0;
		double unusedCyclesB = 0;
		char *spacing = "    ";
		char tempString[20];
		char tempString2[20];
		char tempString3[20];
		printf("%s%s%s%sFIFO%s%s%s%s%s%s%sBANKER'S\n", 
		spacing, spacing, spacing, spacing, spacing, spacing, spacing, spacing, spacing, spacing, spacing);
		for(int i = 0; i  < num_of_tasks; i++)
		{
			double unused = queue[i].totalRuntime - queue[i].cycleTime;
			double percentage = (unused / queue[i].totalRuntime) *100;
			sprintf(tempString, "%d", queue[i].totalRuntime);
			sprintf(tempString2, "%d", (int) unused);
			sprintf(tempString3, "%d", (int) round(percentage));
			printf("%sTask %d", spacing, i+1);
			if(queue[i].aborted == 1)
			{
				printf("         aborted    ");
			}
			else
			{
				unusedCycles += unused;
				runtimeOfAlgo += queue[i].totalRuntime;
				printf("%s%6s%4s%5s%%", spacing, tempString, tempString2, tempString3);
			}
			printf("%s%s", spacing, spacing);
			printf("%sTask %d", spacing, i+1);
			double unusedB = queueB[i].totalRuntime - queueB[i].cycleTime;
			double percentageB = (unused / queueB[i].totalRuntime) *100;
			sprintf(tempString, "%d", queueB[i].totalRuntime);
			sprintf(tempString2, "%d", (int) unusedB);
			sprintf(tempString3, "%d", (int) round(percentageB));
			if(queueB[i].aborted == 1)
			{
				printf("         aborted    \n");
			}
			else
			{
				unusedCyclesB += unusedB;
				runtimeOfAlgoB += queueB[i].totalRuntime;
				printf("%s%6s%4s%4s%%\n", spacing, tempString, tempString2, tempString3);
			}
		}
		double percentageUnused = (unusedCycles / runtimeOfAlgo) *100;
		double percentageUnusedB = (unusedCyclesB / runtimeOfAlgoB) *100;
		sprintf(tempString, "%d", runtimeOfAlgo);
		sprintf(tempString2, "%d", (int) unusedCycles);
		sprintf(tempString3, "%d", (int) round(percentageUnused));
		printf("%stotal     %6s%4s %4s%%%s", spacing, tempString, tempString2, tempString3, spacing);
		sprintf(tempString, "%d", runtimeOfAlgoB);
		sprintf(tempString2, "%d", (int) unusedCyclesB);
		sprintf(tempString3, "%d", (int) round(percentageUnusedB));
		printf("%s%stotal     %6s%4s%4s%%\n", spacing, spacing, tempString, tempString2, tempString3);

		/* tester
		for(int i = 0; i < num_of_tasks; i++) {
			struct node *temp = queue[i].activities;
			while(temp) {
				Activity *a = temp->activity;
				printf("%s %d %d %d\n", a->name, a->pid, a->val2, a->val3);
				temp = temp->next;
			}
		} */
		free(queue);
	}
	return 0;
}