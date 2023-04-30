#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
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
  struct list_item *p = NULL;
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
  struct list_item *new_list_item = NULL;

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
  struct list_item *temp = NULL;

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

/* Vector2 c3_node_positions[3] = {{380, 100}, {315, 195}, {450, 195}}; */

struct graph cyclic_graph_create(int key, int *nodeCount, Color nodeColors[], int nodeDiameter, Vector2 mousePosition) {
  struct graph cyclic_graph;
  cyclic_graph.nodes = NULL;
  cyclic_graph.lines = NULL;

  int colorIndex = 0;
  Vector2 nodePos;

  Vector2 *c3_node_positions = triangle_vertices_from_centroid(mousePosition);

  for (int i = 0; i < key; i++) {
    //create node and add to list
    struct node *new_node = create_node(*nodeCount, colorIndex,
                                        nodeColors, nodeDiameter,
                                        c3_node_positions[i]);
    cyclic_graph.nodes = add_to_list(cyclic_graph.nodes, new_node);

    //update variables
    colorIndex++;
    (*nodeCount)++;
  }
  //create lines and add to list
  struct list_item *temp = cyclic_graph.nodes;
  struct node *startNode = temp->data;
  struct node *endNode = startNode;

  while (temp != NULL && temp->next != NULL) {
    endNode = temp->next->data;
    struct line *new_line = malloc(sizeof(struct line));
    new_line->startNode = *startNode;
    new_line->endNode = *endNode;
    cyclic_graph.lines = add_to_list(cyclic_graph.lines, new_line);
    /* ((struct line *)cyclic_graph.lines->data)->endNode = *(struct node *)temp->data; */
    startNode = endNode;
    temp = temp->next;
  }
  // add the last line connecting the last and first nodes
  endNode = cyclic_graph.nodes->data;
  struct line *new_line = malloc(sizeof(struct line));
  new_line->startNode = *startNode;
  new_line->endNode = *endNode;
  cyclic_graph.lines = add_to_list(cyclic_graph.lines, new_line);

  return cyclic_graph;
}

Vector2 *triangle_vertices_from_centroid(Vector2 centroid) {
  Vector2 *vertices = malloc(3 * sizeof(centroid));
  int side_length = 100;

  vertices[0].x = centroid.x;
  vertices[0].y = centroid.y - (sqrt(3)/3) * side_length;
  vertices[1].x = centroid.x + side_length / 2.0;
  vertices[1].y = centroid.y + (sqrt(3)/6) * side_length;
  vertices[2].x = centroid.x - side_length / 2.0;
  vertices[2].y = vertices[1].y;

  return vertices;
}
