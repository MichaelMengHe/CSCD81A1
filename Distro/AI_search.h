/*
	CSC D84 - Unit 1 - Search

	This file contains the API function headers for your assignment.
	Please pay close attention to the function prototypes, and
	understand what the arguments are.

	Stubs for implementing each function are to be found in AI_search.c,
	along with clear ** TO DO markers to let you know where to add code.

	You are free to add helper functions within reason. But you must
	provide a prototype *in this file* as well as the implementation
	in the .c program file.

	Starter by: F.J.E., Jul. 2015
	Updated by: F.J.E., Jan. 2018
*/

#ifndef __AI_search_header

#define __AI_search_header

// Generally needed includes
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <malloc.h>

#include "board_layout.h"

// Function prototypes for D84 - Unit 1 - Search assignment solution

void search(double gr[graph_size][4], int path[graph_size][2], int visit_order[size_X][size_Y], int cat_loc[10][2], int cats, int cheese_loc[10][2], int cheeses, int mouse_loc[1][2], int mode, int (*heuristic)(int x, int y, int cat_loc[10][2], int cheese_loc[10][2], int mouse_loc[1][2], int cats, int cheeses, double gr[graph_size][4]));
int H_cost(int x, int y, int cat_loc[10][2], int cheese_loc[10][2], int mouse_loc[1][2], int cats, int cheeses, double gr[graph_size][4]);
int H_cost_nokitty(int x, int y, int cat_loc[10][2], int cheese_loc[10][2], int mouse_loc[1][2], int cats, int cheeses, double gr[graph_size][4]);

// If you need to add any function prototypes yourself, you can do so *below* this line.

struct Node
{
	int idx;
	int cost;
	struct Node *next;
};

struct Queue
{
	struct Node *head;
	struct Node *tail;
};

struct Queue *create_queue();
struct Node *create_node(int idx);
void enqueue(struct Queue *queue, int idx, int cost, int is_a_star);
int dequeue(struct Queue *queue);
void free_queue(struct Queue *queue);
int is_empty(struct Queue *queue);

int is_in(int curr_idx, int list[10], int size);

int dfs(int node_index, int *visit_counter, int visit_order[size_X][size_Y], double gr[graph_size][4], int cat_list[10], int cats, int cheese_list[10], int cheeses, int path[graph_size][2]);
void reverse_array(int path[1024][2], int n);
void print_path(int path[graph_size][2], int final_index);
void print_arr(int arr[graph_size]);

#endif