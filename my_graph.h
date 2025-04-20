#ifndef MY_GRAPH_H
#define MY_GRAPH_H

#include <stdbool.h>   /* C99 only */
#include <assert.h>
#include "raylib.h"


struct node {
  int id;             
  int graph_id;   /* 0 means not a part of a graph*/
  int diameter;       
  Color color;        
  Vector2 position;   
  Vector2 originalPosition;
  bool selected;
  bool locked;
};

/* static_assert(sizeof(int) * 2 + sizeof(Color) + sizeof(Vector2) + sizeof(bool), */
/*               "struct node must not have any padding"); */

struct line {
  struct node startNode;
  struct node endNode;
};

struct list_item {
  void *data;
  struct list_item *next;
};

struct graph {
  int id;
  struct list_item *nodes;
  struct list_item *lines;
};

void CustomLog(int msgType, const char *text, va_list args);
bool is_empty(struct list_item *list);
struct list_item *add_to_list(struct list_item *list, void *n);
struct list_item *search_list(struct list_item *list, int n);
void delete_from_list(struct list_item **list, int n);
int count_occurrences(struct list_item *list, int n);
struct list_item *find_last(struct list_item *list, int n);
struct list_item *insert_into_ordered_list(struct list_item *list, struct list_item *new_list_item);
void free_list(struct list_item *list);

struct node *node_create(int nodeCount, int colorIndex, Color nodeColors[], int nodeDiameter, Vector2 mousePosition);
void print_node_list(struct list_item *nodes);

struct line *line_create(struct node *startNode, struct node *endNode);

struct line *start_line(struct node *selectedNode);
void end_line(struct list_item *line_list, struct node *selectedNode);

struct graph cyclic_graph_create(int num_nodes, int *nodeCount, Color nodeColors[], int nodeDiameter, Vector2 mousePosition);

Vector2 *n_vertices_from_centroid(Vector2 centroid, int num_nodes);

bool update_lists(struct graph *graph, struct list_item **node_list, struct list_item **line_list);

struct graph complete_graph_create(int num_nodes, int *nodeCount, Color nodeColors[], int nodeDiameter, Vector2 mousePosition);

int line_exists(struct list_item *lines, struct node *startNode, struct node *endNode);

int nodes_equal(struct node *node1, struct node *node2);

int add_nodes_to_graph(struct node *nodes);
/* Takes nodes to add to graph and returns the id number of the graph */

/* debugging */
void print_lines_list(struct list_item *lines);
#endif
