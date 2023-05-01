#ifndef MY_GRAPH_H
#define MY_GRAPH_H

#include <stdbool.h>   /* C99 only */
#include "raylib.h"

struct node {
  int id;             
  int diameter;       
  Color color;        
  Vector2 position;   
};

struct line {
  struct node startNode;
  struct node endNode;
};

struct list_item {
  void *data;
  struct list_item *next;
};

struct graph {
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

struct node *create_node(int nodeCount, int colorIndex, Color nodeColors[], int nodeDiameter, Vector2 mousePosition);
void print_node_list(struct list_item *nodes);

struct line *start_line(struct node *selectedNode);
void end_line(struct list_item *line_list, struct node *selectedNode);

struct graph cyclic_graph_create(int num_nodes, int *nodeCount, Color nodeColors[], int nodeDiameter, Vector2 mousePosition);

Vector2 *triangle_vertices_from_centroid(Vector2 centroid);

Vector2 *n_vertices_from_centroid(Vector2 centroid, int num_nodes);

void update_lists(struct graph *graph, struct list_item **node_list, struct list_item **line_list); 
#endif
