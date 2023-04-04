#include <stdio.h>
#include <stdlib.h>
#include "my_graph.h"

struct node *create_node(int structCount, int colorIndex, Color nodeColors[],
                        int nodeDiameter, Vector2 mousePosition) {
  struct node *node = malloc(sizeof(struct node));
  node->id = structCount;
  node->color = nodeColors[colorIndex];
  node->diameter = nodeDiameter;
  node->position = mousePosition;

  return node;
}

bool is_empty(struct list_item *list) { return list == NULL; }

struct list_item *add_to_list(struct list_item *list, void *n)
{
  struct list_item *new_list_item;

  new_list_item = malloc(sizeof(struct list_item));
  if (new_list_item == NULL) {
    printf("Error: malloc failed in add_to_list\n");
    exit(EXIT_FAILURE);
  }
  new_list_item->data = n;
  new_list_item->next = list;
  return new_list_item;
}
