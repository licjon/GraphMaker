#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include "my_graph.h"


struct node *node_create(int nodeCount, int colorIndex, Color nodeColors[],
                        int nodeDiameter, Vector2 mousePosition) {
  struct node *node = malloc(sizeof(struct node));
  if (node != NULL) {
    node->id = nodeCount;
    node->color = nodeColors[colorIndex];
    node->diameter = nodeDiameter;
    node->position = mousePosition;
  } else {
    printf("Node is null in node_create\n");
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

void print_lines_list(struct list_item *lines) {
  struct list_item *p = NULL;
  printf("\n");
  if (lines == NULL)
    printf("Empty\n");
                 
  for (p = lines; p != NULL; p = p->next) {
    printf("start node id: %d end node id: %d\n", ((struct line *)p->data)->startNode.id, ((struct line *)p->data)->endNode.id);
  }
}

struct line *line_create(struct node *startNode, struct node *endNode) {
  struct line *new_line = malloc(sizeof(struct line));
  new_line->startNode = *startNode;
  new_line->endNode = *endNode;
  return new_line;
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

struct graph cyclic_graph_create(int num_nodes, int *nodeCount, Color nodeColors[], int nodeDiameter, Vector2 mousePosition) {
  struct graph cyclic_graph;
  cyclic_graph.nodes = NULL;
  cyclic_graph.lines = NULL;

  int colorIndex = 0;

  Vector2 *node_positions = n_vertices_from_centroid(mousePosition, num_nodes);

  for (int i = 0; i < num_nodes; i++) {
    //create node and add to list
    struct node *new_node = node_create(*nodeCount, colorIndex,
                                        nodeColors, nodeDiameter,
                                        node_positions[i]);

    cyclic_graph.nodes = add_to_list(cyclic_graph.nodes, new_node);

    //update variables
    colorIndex++;
    (*nodeCount)++;
  }
  free(node_positions);
  //create lines and add to list
  struct list_item *temp = cyclic_graph.nodes;
  struct node *startNode = temp->data;
  struct node *endNode = startNode;

  while (temp != NULL && temp->next != NULL) {
    endNode = temp->next->data;
    struct line *new_line = line_create(startNode, endNode);
    cyclic_graph.lines = add_to_list(cyclic_graph.lines, new_line);
    startNode = endNode;
    temp = temp->next;
  }
  // add the last line connecting the last and first nodes
  endNode = cyclic_graph.nodes->data;
  struct line *new_line = line_create(startNode, endNode);
  cyclic_graph.lines = add_to_list(cyclic_graph.lines, new_line);

  return cyclic_graph;
}

Vector2 *n_vertices_from_centroid(Vector2 centroid, int num_nodes) {
  Vector2 *vertices = malloc(num_nodes * sizeof(Vector2));
  double radius = 70;
  double angle_increment_value = 2 * M_PI / num_nodes;
  double angle = 0;
  
  if (num_nodes % 2 == 0) { 
    angle = -(M_PI / 2) - (M_PI / num_nodes);
  } else {
    angle = -(M_PI / 2);
  }
  
  for (int i = 0; i < num_nodes; i++) {
     vertices[i].x = centroid.x + cos(angle) * radius;
     vertices[i].y = centroid.y + sin(angle) * radius;
     angle += angle_increment_value;
  }

  return vertices;
}

void update_lists(struct graph *graph, struct list_item **node_list, struct list_item **line_list) {
  struct list_item *temp = graph->nodes;
  struct list_item *temp2 = graph->lines;
  while (temp != NULL && temp2 != NULL) {
    struct list_item* new_node = (struct list_item*)malloc(sizeof(struct list_item));
    new_node->data = temp->data;
    new_node->next = *node_list;
    *node_list = new_node;
    temp = temp->next;
    struct list_item* new_line = (struct list_item*)malloc(sizeof(struct list_item));
    new_line->data = temp2->data;
    new_line->next = *line_list;
    *line_list = new_line;
    temp2 = temp2->next;
  }
  // If node list is longer than line list, add the remaining nodes to node list
  while (temp != NULL) {
    struct list_item* new_node = (struct list_item*)malloc(sizeof(struct list_item));
    new_node->data = temp->data;
    new_node->next = *node_list;
    *node_list = new_node;
    temp = temp->next;
  }
  // If line list is longer than node list, add the remaining lines to line list
  while (temp2 != NULL) {
    struct list_item* new_line = (struct list_item*)malloc(sizeof(struct list_item));
    new_line->data = temp2->data;
    new_line->next = *line_list;
    *line_list = new_line;
    temp2 = temp2->next;
  }
}

struct graph complete_graph_create(int num_nodes, int *nodeCount, Color nodeColors[], int nodeDiameter, Vector2 mousePosition) {
    struct graph complete_graph;
    complete_graph.nodes = NULL;
    complete_graph.lines = NULL;

    int colorIndex = 0;

    // Generate node positions
    Vector2 *nodePositions = n_vertices_from_centroid(mousePosition, num_nodes);

    // Create nodes
    for (int i = 0; i < num_nodes; i++) {
      //create node and add to list
      struct node *new_node = node_create(*nodeCount, colorIndex,
                                          nodeColors, nodeDiameter,
                                          nodePositions[i]);
      
      complete_graph.nodes = add_to_list(complete_graph.nodes, new_node);
      
      //update variables
      colorIndex++;
      (*nodeCount)++;
    }
    // Create remaining lines to make complete graph
    struct list_item *temp1 = complete_graph.nodes;
    while (temp1 != NULL && temp1->next != NULL) {
        struct list_item *temp2 = temp1->next;
        while (temp2 != NULL) {
            struct node *startNode = temp1->data;
            struct node *endNode = temp2->data;
            struct line *new_line = malloc(sizeof(struct line));
            new_line->startNode = *startNode;
            new_line->endNode = *endNode;
            complete_graph.lines = add_to_list(complete_graph.lines, new_line);
            temp2 = temp2->next;
        }
        temp1 = temp1->next;
    }

    free(nodePositions);

    return complete_graph;
}
