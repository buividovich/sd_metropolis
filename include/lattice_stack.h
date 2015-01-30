#ifndef _LATTICE_STACK_H_
#define _LATTICE_STACK_H_

#include <stdlib.h>
#include <stdio.h>
#include <clue_logs.h>
#include <limits.h>

//Stack itself
typedef struct 
{
 int    dim;         //Dimensionality of stack elements
 int    max_nel;     //Max number of elements in stack
 int**  stack;       //sint[i] contains the pointer to the array of DIM sint's
 int*   start;       //seq_start[i]  is the first index of i'th sequence in the stack
 int*   len;         //seq_length[i] is the length of i'th sequence in the stack
 int    top;         //Number of sequences in the stack
 int    nel;         //Total number of elements in the stack
 
} t_lat_stack;

//Some useful macro for addressing stack elements
//_S is the stack, _t is the depth from the top (0 is the topmost element, 1 is the element before etc)
//_i is the number of element in the sequence, 0 is the topmost element (first element in the sequence), 1 is the next element etc.
#define STACK_EL(_S, _i) (_S).stack[(_S).start[(_S).top - 1] + (_S).len[(_S).top - 1] - (_i) - 1]

void init_lat_stack(t_lat_stack* lat_stack, int dim, int max_nel);
void free_lat_stack(t_lat_stack* lat_stack);

int   check_stack_consistency(t_lat_stack* lat_stack, const char* stack_name);

#endif
