/*
	CSC D84 - Unit 1 - Search

	This file contains stubs for implementing the different search
	algorithms covered in the course. Please read the assignment
	handout carefully - it describes the game, the data you will
	have to handle, and the search functions you must provide.

	Once you have read the handout carefully, implement your search
	code in the sections below marked with

	**************
	*** TO DO:
	**************

	Make sure to add it to your report.txt file - it will be marked!

	Have fun!

	DO NOT FORGET TO 'valgrind' YOUR CODE - We will check for pointer
	management being done properly, and for memory leaks.

	Starter code: F.J.E., Jul. 15
	Updated: F.J.E., Jan. 18
*/

/**********************************************************************
% COMPLETE THIS TEXT BOX:
%
% 1) Student Name: Meng He		
% 2) Student Name:		
%
% 1) Student number: 1004181146
% 2) Student number:
% 
% 1) UtorID hemeng2
% 2) UtorID
% 
% We hereby certify that the work contained here is our own
%
% Meng He_____________             Mohamed Tayeh
% (sign with your name)            (sign with your name)
***********************************************************************/

#include "AI_search.h"

void search(double gr[graph_size][4], int path[graph_size][2], int visit_order[size_X][size_Y], int cat_loc[10][2], int cats, int cheese_loc[10][2], int cheeses, int mouse_loc[1][2], int mode, int (*heuristic)(int x, int y, int cat_loc[10][2], int cheese_loc[10][2], int mouse_loc[1][2], int cats, int cheeses, double gr[graph_size][4]))
{
	/*
   This function is the interface between your solution for the assignment and the driver code. The driver code
   in AI_search_core_GL will call this function once per frame, and provide the following data
   
   Board and game layout:
	
	The game takes place on a grid of size 32x32, the file board_layout.h specifies the size and defines two
	constants 'size_X' and 'size_Y' for the horizontal and vertical size of the board, respectively. For our
	purposes, the grid of locations is represented by a graph with one node per grid location, so given
	the 32x32 cells, the graph has 1024 nodes. 

	To create a maze, we connect cell locations in the grid in such a way that a) there is a path from any
	grid location to any other grid location (i.e. there are no disconnected subsets of nodes in the graph),
	and b) there are loops.

	Since each node represents a grid location, each node can be connected to up to 4 neighbours in the
	top, right, bottom, and left directions respectively:

			node at (i,j-1)
				^
				|
	(node at i-1, j) <- node at (i,j) -> node at (i+1, j)
				|
				v
			node at (i,j+1)

	The graph is theredore stored as an adjacency list with size 1024 x 4, with one row per node in the
	graph, and 4 columns corresponding to the weight of an edge linking the node with each of its 4
	possible neighbours in the order towp, right, bottom, left (clockwise from top).

	Since all we care is whether nodes are connected. Weights will be either 0 or 1, if the weight is
	1, then the neighbouring nodes are connected, if the weight is 0, they are not. For example, if

	graph[i][0] = 0 
	graph[i][1] = 1
	graph[i][2] = 0
	graph[i][3] = 1

	then node i is connected to the right and left neighbours, but not to top or bottom.

	The index in the graph for the node corresponding to grid location (x,y) is

	index = x + (y*size_X) 		or in this case		index = x + (y*32)

	Conversely, if you have the index and want to figure out the grid location,

	x = index % size_X		or in this case		x = index % 32
	y = index / size_Y		or in this case		y = index / 32

	(all of the above are *integer* operations!)

	A path is a sequence of (x,y) grid locations. We store it using an array of size	
	1024 x 2 (since there are 1024 locations, this is the maximum length of any
	path that visits locations only once).

	Agent locations are coordinate pairs (x,y)	

   Arguments:
		gr[graph_size][4]   - This is an adjacency list for the maze
		path[graph_size][2] - An initially empty path for your code to fill.
				      In this case, empty means all entries are initially -1
		visit_order[size_X][size_Y]	- An array in which your code will store the
						  *order* in which grid locations were 
						  visited during search. For example, while
						  doing BFS, the initial location is the
						  start location, it's visit order is 1.
						  Then the search would expand the immediate
						  neighbours of the initial node in some order,
						  these would get a visit order of 2, 3, 4, and
						  5 respectively, and so on.
					
						  This array will be used to display the search
						  pattern generated by each search method.

		cat_loc[10][2], cats   - Location of cats and number of cats (we can have at most 10,
					 but there can be fewer). Only valid cat locations are 0 to (cats-1)
		cheese_loc[10][2], cheeses - Location and number of cheese chunks (again at most 10,
					     but possibly fewer). Valid locations are 0 to (cheeses-1)
		mouse_loc[1][2] - Mouse location - there can be only one!
		mode - Search mode selection:
					mode = 0 	- BFS
					mode = 1	- DFS
					mode = 2	- A*

		(*heuristic)(int x, int y, int cat_loc[10][2], int cheese_loc[10][2], int mouse_loc[10][2], int cats, int cheeses)
				- This is a pointer to one of the heuristic functions you will implement, either H_cost()
				  or H_cost_nokitty(). The driver in AI_search_core_GL will pass the appropriate pointer
				  depending on what search the user wants to run.
			
				  If the mode is 0 or 1, this pointer is NULL

				  * How to call the heuristic function from within this function : *
					- Like any other function:
						h = heuristic( x, y, cat_loc, cheese_loc, mouse_loc, cats, cheeses);

   Return values:
		Your search code will directly update data passed-in as arguments:

		- path[graph_size][2]	: Your search code will update this array to contain the path from
					  the mouse to one of the cheese chunks. The order matters, so
					  path[0][:] must be the mouse's current location, path[1][:]
					  is the next move for the mouse. Each successive row will contain
					  the next move toward the cheese, and the path ends at a location
					  whose coordinates correspond to one of the cheese chunks.
					  Any entries beyond that must remain set to -1
		- visit_order[size_X][size_Y] 	:  Your search code will update this array to contain the
						   order in which each location in the grid was expanded
						   during search. This means, when that particular location
						   was checked for being a goal, and if not a goal, had its
						   neighbours added as candidates for future expansion (in
						   whatever order is dictated by the search mode).

						   Note that since there are 1024 locations, the values in 
						   this array must always be in [0, 1023]. The driver code
						   will then display search order as a yellow-colored
						   brightness map where nodes expanded earlier will look
						   brighter.

		* Your code MUST NOT modify the locations or numbers of cats and/or cheeses, the graph,
	     	  or the location of the mouse - if you try, the driver code will know it *
			
		That's that, now, implement your solution!
 */

	/********************************************************************************************************
 * 
 * TO DO:	Implement code to carry out the different types of search depending on the
 *		mode.
 *
 *		You can do this by writing code within this single function (and being clever about it!)
 *		Or, you can use this function as a wrapper that calls a different search function
 *		 (BFS, DFS, A*) depending on the mode. Note that in the latter case, you will have
 *	    	  to inform your A* function somehow of what heuristic it's supposed to use.
 *
 *		Visiting Order: When adding the neighbours of a node to your list of candidates for
 *				expansion, do so in the order top, right, bottom, left.
 *
 *		NOTE: Your search functions should be smart enough to not choose a path that goes
 *		      through a cat! this is easily done without any heuristics.
 * 
 *		How you design your solution is up to you. But:
 *
 *		- Document your implementation by adding concise and clear comments in this file
 *		- Document your design (how you implemented the solution, and why) in the report
 *
 ********************************************************************************************************/

	// Stub so that the code compiles/runs - The code below will be removed and replaced by your code!

	// assign the info about cheese and cats to usse later
	int cheese_list[10];
	int cat_list[10];
	// this gets the index of the mouse
	int mouse_index = mouse_loc[0][0] + (mouse_loc[0][1] * size_X);

	for (int x = 0; x < cheeses; x++)
	{
		cheese_list[x] = cheese_loc[x][0] + (cheese_loc[x][1] * size_X);
	}

	for (int y = 0; y < cats; y++)
	{
		cat_list[y] = cat_loc[y][0] + (cat_loc[y][1] * size_X);
	}

	int visit_counter = 0;

	// Mode 0 i BFS
	if (mode == 0)
	{
		// set curr index and make a queue for BFS
		int curr_index;
		struct Queue *queue = create_queue();

		// starting point is the mouse index
		enqueue(queue, mouse_index, 0, 0);
		visit_order[mouse_loc[0][0]][mouse_loc[0][1]] = visit_counter;

		// inititate counters for loop
		visit_counter = 1;
		int curr_x;
		int curr_y;
		int new_x;
		int new_y;
		int new_index;
		int i;

		// Predecessor book keeping
		int prev_level[graph_size][2];
		prev_level[mouse_index][0] = 0;
		prev_level[mouse_index][1] = 0;

		// loop until mouse reaches cheese
		do
		{
			// get next node from queue
			curr_index = dequeue(queue);
			if (curr_index == -1)
			{
				free_queue(queue);
				return;
			}
			curr_x = curr_index % size_X;
			curr_y = curr_index / size_Y;
			// expand neighbours
			if (is_in(curr_index, cheese_list, cheeses) == 0)
			{
				for (i = 0; i < 4; i++)
				{
					if (gr[curr_index][i] == (double)1)
					{
						// Add to queue if there exists a weight between them
						if (i == 0)
						{
							new_x = curr_x;
							new_y = curr_y - 1;
						}
						else if (i == 1)
						{
							new_x = curr_x + 1;
							new_y = curr_y;
						}
						else if (i == 2)
						{
							new_x = curr_x;
							new_y = curr_y + 1;
						}
						else if (i == 3)
						{
							new_x = curr_x - 1;
							new_y = curr_y;
						}
						if (visit_order[new_x][new_y] == 0 && is_in((new_index = new_x + (new_y * size_X)), cat_list, cats) == 0)
						{
							enqueue(queue, new_index, 0, 0);
							prev_level[new_index][0] = curr_index;
							prev_level[new_index][1] = prev_level[curr_index][1] + 1;
							// Add to visited
							visit_order[new_x][new_y] = visit_counter;
							visit_counter++;
						}
					}
				}
			}
		} while (is_in(curr_index, cheese_list, cheeses) == 0 && !is_empty(queue));

		if (is_in(curr_index, cheese_list, cheeses) == 1)
		{
			// Build the path of the mouse using predecessors
			for (int j = prev_level[curr_index][1]; j >= 0; j--)
			{
				path[j][0] = curr_index % size_X;
				path[j][1] = curr_index / size_Y;
				curr_index = prev_level[curr_index][0];
			}
		}
		else
		{
			for (i = 0; i < graph_size; i++)
			{
				path[i][0] = mouse_loc[0][0];
				path[i][1] = mouse_loc[0][1];
			}
		}

		free_queue(queue);
	}
	else if (mode == 1)
	{

		int final_index = dfs(mouse_index, &visit_counter, visit_order, gr, cat_list, cats, cheese_list, cheeses, path);

		path[final_index][0] = mouse_loc[0][0];
		path[final_index][1] = mouse_loc[0][1];

		// Path array was written in reverse order during the recurssion, so reverse again
		reverse_array(path, final_index);
	}
	else if (mode == 2)
	{
		// Modified BFS code
		printf("mouse index %d %d \n", mouse_loc[0][0], mouse_loc[0][1]);

		// Cost tracker to nodes expanded so far
		// Initial cost to get to mouse's position is 0
		int cost[graph_size];
		cost[mouse_index] = 0;

		// Set curr index and make a queue for BFS
		int curr_index;
		struct Queue *queue = create_queue();

		// Starting point is the mouse index
		int mouse_loc_x = mouse_loc[0][0];
		int mouse_loc_y = mouse_loc[0][1];
		int mouse_h_cost = heuristic(mouse_loc_x, mouse_loc_y, cat_loc, cheese_loc, mouse_loc, cats, cheeses, gr);

		enqueue(queue, mouse_index, mouse_h_cost, 1);
		visit_order[mouse_loc_x][mouse_loc_y] = visit_counter;

		// Initiate counters for loop
		visit_counter = 1;
		int curr_x;
		int curr_y;
		int new_x;
		int new_y;
		int new_index;
		int i;

		int prev_level[graph_size][2];
		prev_level[mouse_index][0] = 0;
		prev_level[mouse_index][1] = 0;

		// Loop until mouse reaches cheese
		do
		{
			// Get next node from queue
			curr_index = dequeue(queue);
			if (curr_index == -1)
			{
				free_queue(queue);
				return;
			}
			curr_x = curr_index % size_X;
			curr_y = curr_index / size_Y;

			// Expand neighbours
			if (is_in(curr_index, cheese_list, cheeses) == 0)
			{
				for (i = 0; i < 4; i++)
				{
					if (gr[curr_index][i] == (double)1)
					{
						// Add to queue if there exists a weight between them
						if (i == 0)
						{
							new_x = curr_x;
							new_y = curr_y - 1;
						}
						else if (i == 1)
						{
							new_x = curr_x + 1;
							new_y = curr_y;
						}
						else if (i == 2)
						{
							new_x = curr_x;
							new_y = curr_y + 1;
						}
						else if (i == 3)
						{
							new_x = curr_x - 1;
							new_y = curr_y;
						}

						int new_index = new_x + (new_y * size_X);

						if (visit_order[new_x][new_y] == 0 && is_in(new_index, cat_list, cats) == 0)
						{
							// Adding to PQ
							int cost_to_new_node = cost[curr_index] + 1;
							cost[new_index] = cost_to_new_node;

							int new_h_cost = heuristic(new_x, new_y, cat_loc, cheese_loc, mouse_loc, cats, cheeses, gr);

							int new_cost = cost_to_new_node + new_h_cost;
							enqueue(queue, new_index, new_cost, 1);

							// Tracking preds
							prev_level[new_index][0] = curr_index;
							prev_level[new_index][1] = prev_level[curr_index][1] + 1;
						}
						// Add to visited
						visit_order[new_x][new_y] = visit_counter;
						visit_counter++;
					}
				}
			}
		} while (is_in(curr_index, cheese_list, cheeses) == 0 && !is_empty(queue));

		if (is_in(curr_index, cheese_list, cheeses) == 1)
		{
			// Build the path of the mouse using predecessors
			for (int j = prev_level[curr_index][1]; j >= 0; j--)
			{
				path[j][0] = curr_index % size_X;
				path[j][1] = curr_index / size_Y;
				curr_index = prev_level[curr_index][0];
			}
		}
		else
		{
			for (i = 0; i < graph_size; i++)
			{
				path[i][0] = mouse_loc[0][0];
				path[i][1] = mouse_loc[0][1];
			}
		}

		free_queue(queue);
	}
	return;
}

int H_cost(int x, int y, int cat_loc[10][2], int cheese_loc[10][2], int mouse_loc[1][2], int cats, int cheeses, double gr[graph_size][4])
{
	/*
	This function computes and returns the heuristic cost for location x,y.
	As discussed in lecture, this means estimating the cost of getting from x,y to the goal. 
	The goal is cheese. Which cheese is up to you.
	Whatever you code here, your heuristic must be admissible.

	Input arguments:

		x,y - Location for which this function will compute a heuristic search cost
		cat_loc - Cat locations
		cheese_loc - Cheese locations
		mouse_loc - Mouse location
		cats - # of cats
		cheeses - # of cheeses
		gr - The graph's adjacency list for the maze

		These arguments are as described in the search() function above
 */

	// Use Manhattan distance
	int least_cost = -1;
	int manhattan;

	// Loop through each cheese and calculate its manhattan distance from (x, y)
	for (int i = 0; i < cheeses; i++)
	{
		manhattan = abs(x - cheese_loc[i][0]) + abs(y - cheese_loc[i][1]);
		if (manhattan < least_cost || least_cost == -1)
		{
			least_cost = manhattan;
		}
	}

	// return the least cost
	return least_cost;
}

int H_cost_nokitty(int x, int y, int cat_loc[10][2], int cheese_loc[10][2], int mouse_loc[1][2], int cats, int cheeses, double gr[graph_size][4])
{
	/*
	This function computes and returns the heuristic cost for location x,y.
	As discussed in lecture, this means estimating the cost of getting from x,y to the goal. 
	The goal is cheese. 

	However - this time you want your heuristic function to help the mouse avoid being eaten.
	Therefore: You have to somehow incorporate knowledge of the cats' locations into your
	heuristic cost estimate. How well you do this will determine how well your mouse behaves
	and how good it is at escaping kitties.

	This heuristic *does not have to* be admissible.

	Input arguments have the same meaning as in the H_cost() function above.
 */
	// Take both cats locations and cheese locations into account
	int manhattan;
	int closest_cat = -1;
	int manhattan_closest_cheese = -1;
	int max_manhattan = size_X + size_Y - 2;
	// this gets the closet cat
	for (int i = 0; i < cats; i++)
	{
		manhattan = abs(x - cat_loc[i][0]) + abs(y - cat_loc[i][1]);
		if (manhattan < closest_cat || closest_cat == -1)
		{
			closest_cat = manhattan;
		}
	}
	// this gets the closest cheese
	for (int j = 0; j < cheeses; j++)
	{
		manhattan = abs(x - cheese_loc[j][0]) + abs(y - cheese_loc[j][1]);
		if (manhattan < manhattan_closest_cheese || manhattan_closest_cheese == -1)
		{
			manhattan_closest_cheese = manhattan;
		}
	}

	return manhattan_closest_cheese + (max_manhattan - closest_cat) * (max_manhattan - closest_cat);
}

struct Queue *create_queue()
{
	struct Queue *queue = (struct Queue *)malloc(sizeof(struct Queue));
	queue->head = queue->tail = NULL;
	return queue;
}

struct Node *create_node(int idx, int cost)
{
	struct Node *new_node = (struct Node *)malloc(sizeof(struct Node));
	new_node->idx = idx;
	new_node->cost = cost;
	new_node->next = NULL;
	return new_node;
}

void enqueue(struct Queue *queue, int idx, int cost, int is_a_star)
{
	struct Node *new_node = create_node(idx, cost);

	// Queue is empty therefore we add it
	if (queue->tail == NULL)
	{
		queue->head = queue->tail = new_node;
		return;
	}
	else
	{
		// Queue is not empty
		if (is_a_star)
		{
			struct Node *start = queue->head;

			if (start->cost >= cost)
			{
				// Insert New Node before head
				new_node->next = start;
				queue->head = new_node;
			}
			else
			{

				// Traverse the list and find a
				// position to insert new node
				while (start->next != NULL &&
					   start->next->cost <= cost)
				{
					start = start->next;
				}

				// Either at the ends of the list
				// or at required position
				new_node->next = start->next;
				start->next = new_node;
			}
		}
		else
		{
			queue->tail->next = new_node;
			queue->tail = new_node;
		}
	}
	return;
}

int dequeue(struct Queue *queue)
{
	if (queue->head == NULL)
	{
		return -1;
	}

	struct Node *old_head = queue->head;

	queue->head = old_head->next;

	int idx = old_head->idx;

	if (queue->head == NULL)
	{
		queue->tail = NULL;
	}

	free(old_head);
	return idx;
}

void free_queue(struct Queue *queue)
{
	if (queue->head == NULL)
	{
		free(queue);
	}
	else
	{
		struct Node *temp = queue->head;
		struct Node *temp2;
		while ((temp2 = temp->next) != NULL)
		{
			free(temp);
			temp = temp2;
		}
		free(temp);
	}
	return;
}

int is_empty(struct Queue *queue)
{

	if (queue->head == NULL)
	{
		return 1;
	}

	return 0;
}

int is_in(int curr_idx, int list[10], int size)
{
	for (int i = 0; i < size; i++)
	{
		if (curr_idx == list[i])
		{
			return 1;
		}
	}
	return 0;
}

int dfs(int node_index, int *visit_counter, int visit_order[size_X][size_Y], double gr[graph_size][4], int cat_list[10], int cats, int cheese_list[10], int cheeses, int path[graph_size][2])
{
	// Curr mouse coordinates
	int curr_x = node_index % size_X;
	int curr_y = node_index / size_Y;

	// Base case - it found the cheese
	if (is_in(node_index, cheese_list, cheeses) == 1)
	{
		path[0][0] = curr_x;
		path[0][1] = curr_y;
		return 1;
	}

	// Recursive case - it didn't find the cheese
	visit_order[curr_x][curr_y] = *visit_counter;
	(*visit_counter) = (*visit_counter) + 1; // Update for the next call

	// Go through all the edges
	for (int i = 0; i < 4; i++)
	{
		if (gr[node_index][i] == (double)1)
		{
			// Converts to x, y coordinates
			int new_x;
			int new_y;

			if (i == 0)
			{
				new_x = curr_x;
				new_y = curr_y - 1;
			}
			else if (i == 1)
			{
				new_x = curr_x + 1;
				new_y = curr_y;
			}
			else if (i == 2)
			{
				new_x = curr_x;
				new_y = curr_y + 1;
			}
			else if (i == 3)
			{
				new_x = curr_x - 1;
				new_y = curr_y;
			}

			int next_path_index;
			int next_node_index = (new_x + (new_y * size_X));

			// First time visit and there is no cat
			if (visit_order[new_x][new_y] == 0 && is_in(next_node_index, cat_list, cats) == 0)
			{
				next_path_index = dfs(next_node_index, visit_counter, visit_order, gr, cat_list, cats, cheese_list, cheeses, path);
				if (next_path_index != 0)
				{
					path[next_path_index][0] = curr_x;
					path[next_path_index][1] = curr_y;
					return next_path_index + 1;
				}
			}
		}
	}

	return 0;
}

// Used to revert path array
void reverse_array(int path[graph_size][2], int final_index)
{
	for (int i = 0, j = final_index; i < j; i++, j--)
	{
		int temp[1][2];
		temp[0][0] = path[i][0];
		temp[0][1] = path[i][1];

		path[i][0] = path[j][0];
		path[i][1] = path[j][1];

		path[j][0] = temp[0][0];
		path[j][1] = temp[0][1];
	}
}

// Used to print an array
void print_arr(int arr[graph_size])
{
	printf("Printing array \n");

	for (int i = 0; i < graph_size; i++)
	{
		printf("%d ", arr[i]);
	}
}

// Used to print path
void print_path(int path[graph_size][2], int final_index)
{
	for (int i = 0; i < final_index; i++)
	{
		printf("%d %d", path[i][0], path[i][1]);
	}
}