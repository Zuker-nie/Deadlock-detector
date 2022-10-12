/**
 *  list.h
 *
 *  Full Name:
 *  Course section:
 *  Description of the program: list data structure containing the tasks in the system
 *  
 */

#ifndef LIST_H
#define LIST_H

#include "activity.h"

struct node {
    Activity *activity;
    struct node *next;
};

// insert and delete operations.
void insert(struct node **head, Activity *activity);
void insertTail(struct node **head, Activity *activity);
void delete(struct node **head, Activity *activity);
void traverse(struct node *head);

#endif