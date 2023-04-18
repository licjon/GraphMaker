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

void CustomLog(int msgType, const char *text, va_list args);
bool is_empty(struct list_item *list);
struct list_item *add_to_list(struct list_item *list, void *n);
struct list_item *search_list(struct list_item *list, int n);
void delete_from_list(struct list_item **list, int n);
int count_occurrences(struct list_item *list, int n);
struct list_item *find_last(struct list_item *list, int n);
struct list_item *insert_into_ordered_list(struct list_item *list, struct list_item *new_list_item);


struct node *create_node(int nodeCount, int colorIndex, Color nodeColors[], int nodeDiameter, Vector2 mousePosition);

struct line *start_line(struct node *selectedNode); 
void end_line(struct list_item *line_list, struct node *selectedNode); 

#endif
