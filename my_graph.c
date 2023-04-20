#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "my_graph.h"

struct node *create_node(int nodeCount, int colorIndex, Color nodeColors[],
                        int nodeDiameter, Vector2 mousePosition) {
  struct node *node = malloc(sizeof(struct node));
  if (node != NULL) {
    node->id = nodeCount;
    node->color = nodeColors[colorIndex];
    node->diameter = nodeDiameter;
    node->position = mousePosition;
  } else {
    printf("Node is null in create_node\n");
    exit(1);
  }
  return node;
}

void print_node_list(struct list_item *node_list) {
  struct list_item *p;
  printf("Node list: \n");
  if (node_list == NULL)
    printf("Empty\n");
                 
  for (p = node_list; p != NULL; p = p->next) {
    printf("id: %d\nx: %f\ny: %f\n\n", ((struct node *)p->data)->id, ((struct node *)p->data)->position.x, ((struct node *)p->data)->position.y);
  }
}

struct line *start_line(struct node *selectedNode) {
  struct line *new_line = malloc(sizeof(struct line));
  new_line->startNode = *selectedNode;
  new_line->endNode.position = new_line->startNode.position;
  return new_line;
}

void end_line(struct list_item *line_list, struct node *selectedNode) {
  ((struct line *)line_list->data)->endNode = *selectedNode;
}

bool is_empty(struct list_item *list) { return list == NULL; }

// Add to head of list
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

void free_list(struct list_item *list) {
  struct list_item *temp;

  while (list != NULL) {
    temp = list;
    list = list->next;
    free(temp);
  }
}

void CustomLog(int msgType, const char *text, va_list args)
{
    char timeStr[64] = { 0 };
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);

    strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", tm_info);
    printf("[%s] ", timeStr);

    switch (msgType)
    {
        case LOG_INFO: printf("[INFO] : "); break;
        case LOG_ERROR: printf("[ERROR]: "); break;
        case LOG_WARNING: printf("[WARN] : "); break;
        case LOG_DEBUG: printf("[DEBUG]: "); break;
        default: break;
    }

    vprintf(text, args);
    printf("\n");
}

/* typedef void (*cyclic_graph_ptr) (struct graph *graph); */
/* cyclic_graph_ptr cyclic_graph_funs[7] = { */
/*   c3_graph, c4_graph, c5_graph, c6_graph, c7_graph, c8_graph, c9_graph}; */

Vector2 c3_node_positions[3] = {{380, 100}, {315, 195}, {450, 195}};

struct graph cyclic_graph_create(int key, int *nodeCount, Color nodeColors[], int nodeDiameter) {
  printf("cyclic_graph entered\n");
  struct graph cyclic_graph;
  cyclic_graph.nodes = NULL;
  cyclic_graph.lines = NULL;

  int colorIndex = 0;
  Vector2 nodePos;
  
  /* Vector2 node_positions[9] = {{0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}}; */

  for (int i = 0; i < key; i++) {

    printf("2nd for loop\n");
    struct node *new_node = create_node(*nodeCount, colorIndex,
                                        nodeColors, nodeDiameter,
                                        c3_node_positions[i]);
    
    cyclic_graph.nodes = add_to_list(cyclic_graph.nodes, new_node);
    colorIndex++;
    (*nodeCount)++;
  }

  print_node_list(cyclic_graph.nodes);
  return cyclic_graph;
}

/* void c3_graph(struct graph *g3) { */
/*   g3 = malloc(sizeof(struct graph)); */
/*   printf("3\n"); */
/*   } */

/* struct graph c4_graph(void){struct graph g3; printf("4\n"); return g3;} */
/* struct graph c5_graph(void){struct graph g3; printf("5\n"); return g3;} */
/* struct graph c6_graph(void){struct graph g3;printf("6\n");  return g3;} */
/* struct graph c7_graph(void){struct graph g3;printf("7\n");  return g3;} */
/* struct graph c8_graph(void){struct graph g3;printf("8\n");  return g3;} */
/* struct graph c9_graph(void){struct graph g3;printf("9\n");  return g3;} */
