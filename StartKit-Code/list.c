/**
 *  list.c
 *
 *  Full Name:
 *  Course section:
 *  Description of the program: Various list operations
 *  
 */
 
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "list.h"
#include "activity.h"


// add a new node to the list 
void insert(struct node **head, Activity *newActivity) {
    // add the new activity to the list 
    struct node *newNode = malloc(sizeof(struct node));

    newNode->activity = newActivity;
    newNode->next = *head;
    *head = newNode;
}

// add new node to the tail of the list
void insertTail(struct node **tail, Activity *newActivity) {
    struct node *newNode = malloc(sizeof(struct node));
    newNode->activity = newActivity;
    (*tail)->next = newNode;
    *tail = newNode;
}

// delete the selected now from the list
void delete(struct node **head, Activity *activity) {
    struct node *temp;
    struct node *prev;

    temp = *head;
    // special case - beginning of list
    if (activity->pid ==temp->activity->pid) {
        *head = (*head)->next;
    }
    else {
        // interior or last element in the list
        prev = *head;
        temp = temp->next;
        while ((activity->pid !=temp->activity->pid) ) {
            prev = temp;
            temp = temp->next;
        }

        prev->next = temp->next;
    }
}

// traverse the list
void traverse(struct node *head) {
    struct node *temp;
    temp = head;

    while (temp != NULL) {
        printf("[%d] [%s]\n", temp->activity->pid, temp->activity->name);
        temp = temp->next;
    }
}
