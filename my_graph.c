#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include "my_graph.h"

// Define M_PI if it's not already defined (needed for some platforms)
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

int next_graph_id = 1;

struct node *node_create(int nodeCount, int colorIndex, Color nodeColors[],
                        int nodeDiameter, Vector2 mousePosition) {
  if (nodeColors == NULL) {
    fprintf(stderr, "Error: NULL nodeColors array passed to node_create\n");
    return NULL;
  }
  
  if (nodeDiameter <= 0) {
    fprintf(stderr, "Error: Invalid node diameter value (%d) passed to node_create\n", nodeDiameter);
    return NULL;
  }
  
  struct node *node = malloc(sizeof(struct node));
  if (node == NULL) {
    fprintf(stderr, "Error: Memory allocation failed in node_create\n");
    return NULL;
  }
  
  node->id = nodeCount;
  node->graph_id = 0;
  node->color = nodeColors[colorIndex];
  node->diameter = nodeDiameter;
  node->position = mousePosition;
  node->originalPosition = mousePosition;
  node->selected = false;
  node->locked = false;
  
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
  if (startNode == NULL || endNode == NULL) {
    fprintf(stderr, "Error: NULL node passed to line_create\n");
    return NULL;
  }
  
  struct line *new_line = malloc(sizeof(struct line));
  if (new_line == NULL) {
    fprintf(stderr, "Error: Memory allocation failed in line_create\n");
    return NULL;
  }
  
  new_line->startNode = *startNode;
  new_line->endNode = *endNode;
  return new_line;
}

struct line *start_line(struct node *selectedNode) {
  if (selectedNode == NULL) {
    fprintf(stderr, "Error: NULL node passed to start_line\n");
    return NULL;
  }
  
  struct line *new_line = malloc(sizeof(struct line));
  if (new_line == NULL) {
    fprintf(stderr, "Error: Memory allocation failed in start_line\n");
    return NULL;
  }
  
  new_line->startNode = *selectedNode;
  new_line->endNode.position = new_line->startNode.position;
  return new_line;
}

void end_line(struct list_item *line_list, struct node *selectedNode) {
  if (line_list == NULL || selectedNode == NULL) {
    fprintf(stderr, "Error: NULL parameter passed to end_line\n");
    return;
  }
  
  if (line_list->data == NULL) {
    fprintf(stderr, "Error: Line list data is NULL in end_line\n");
    return;
  }
  
  ((struct line *)line_list->data)->endNode = *selectedNode;
}

bool is_empty(struct list_item *list) { return list == NULL; }

// Add to head of list
struct list_item *add_to_list(struct list_item *list, void *n)
{
  if (n == NULL) {
    fprintf(stderr, "Error: NULL data passed to add_to_list\n");
    return list;  // Return unchanged list
  }

  struct list_item *new_list_item = NULL;

  new_list_item = malloc(sizeof(struct list_item));
  if (new_list_item == NULL) {
    fprintf(stderr, "Error: Memory allocation failed in add_to_list\n");
    return list;  // Return unchanged list
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
  cyclic_graph.id = next_graph_id;

  if (num_nodes <= 0 || nodeCount == NULL || nodeColors == NULL) {
    fprintf(stderr, "Error: Invalid parameters to cyclic_graph_create\n");
    return cyclic_graph;  // Return empty graph
  }

  int colorIndex = 0;

  Vector2 *node_positions = n_vertices_from_centroid(mousePosition, num_nodes);
  if (node_positions == NULL) {
    fprintf(stderr, "Error: Failed to generate node positions for cyclic graph\n");
    return cyclic_graph;  // Return empty graph
  }

  for (int i = 0; i < num_nodes; i++) {
    //create node and add to list
    struct node *new_node = node_create(*nodeCount, colorIndex,
                                        nodeColors, nodeDiameter,
                                        node_positions[i]);
    if (new_node == NULL) {
      fprintf(stderr, "Error: Failed to create node for cyclic graph\n");
      free(node_positions);
      
      // Free already created nodes
      struct list_item *temp = cyclic_graph.nodes;
      while (temp != NULL) {
        struct list_item *next = temp->next;
        free(temp->data);
        free(temp);
        temp = next;
      }
      
      cyclic_graph.nodes = NULL;
      return cyclic_graph;  // Return empty graph
    }
    
    new_node->graph_id = next_graph_id;

    cyclic_graph.nodes = add_to_list(cyclic_graph.nodes, new_node);

    //update variables
    colorIndex++;
    (*nodeCount)++;
  }
  free(node_positions);
  
  // Check if any nodes were actually created
  if (cyclic_graph.nodes == NULL) {
    fprintf(stderr, "Error: No nodes created for cyclic graph\n");
    return cyclic_graph;  // Return empty graph
  }
  
  //create lines and add to list
  struct list_item *temp = cyclic_graph.nodes;
  struct node *startNode = temp->data;
  struct node *endNode = startNode;

  while (temp != NULL && temp->next != NULL) {
    endNode = temp->next->data;
    struct line *new_line = line_create(startNode, endNode);
    if (new_line == NULL) {
      fprintf(stderr, "Error: Failed to create line for cyclic graph\n");
      // Continue despite this error
      startNode = endNode;
      temp = temp->next;
      continue;
    }
    
    cyclic_graph.lines = add_to_list(cyclic_graph.lines, new_line);
    startNode = endNode;
    temp = temp->next;
  }
  
  // add the last line connecting the last and first nodes
  endNode = cyclic_graph.nodes->data;
  struct line *new_line = line_create(startNode, endNode);
  if (new_line != NULL) {
    cyclic_graph.lines = add_to_list(cyclic_graph.lines, new_line);
  } else {
    fprintf(stderr, "Error: Failed to create final line for cyclic graph\n");
  }

  next_graph_id++;
  return cyclic_graph;
}

Vector2 *n_vertices_from_centroid(Vector2 centroid, int num_nodes) {
  if (num_nodes <= 0) {
    fprintf(stderr, "Error: Invalid number of nodes (%d) in n_vertices_from_centroid\n", num_nodes);
    return NULL;
  }

  Vector2 *vertices = malloc(num_nodes * sizeof(Vector2));
  if (vertices == NULL) {
    fprintf(stderr, "Error: Memory allocation failed in n_vertices_from_centroid\n");
    return NULL;
  }
  
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

bool update_lists(struct graph *graph, struct list_item **node_list, struct list_item **line_list) {
  if (graph == NULL || node_list == NULL || line_list == NULL) {
    fprintf(stderr, "Error: NULL pointer passed to update_lists\n");
    return false;
  }

  printf("\nDEBUG: Updating lists from graph (id=%d)\n", graph->id);

  // Count nodes and lines in the graph
  int graph_node_count = 0;
  int graph_line_count = 0;
  struct list_item *count_ptr;
  
  for (count_ptr = graph->nodes; count_ptr != NULL; count_ptr = count_ptr->next) {
    graph_node_count++;
  }
  
  for (count_ptr = graph->lines; count_ptr != NULL; count_ptr = count_ptr->next) {
    graph_line_count++;
  }
  
  printf("DEBUG: Graph contains %d nodes and %d lines\n", graph_node_count, graph_line_count);

  // Check main lists before update
  int main_node_count = 0;
  int main_line_count = 0;
  
  for (count_ptr = *node_list; count_ptr != NULL; count_ptr = count_ptr->next) {
    main_node_count++;
  }
  
  for (count_ptr = *line_list; count_ptr != NULL; count_ptr = count_ptr->next) {
    main_line_count++;
  }
  
  printf("DEBUG: Main lists before update: %d nodes and %d lines\n", 
         main_node_count, main_line_count);

  // First, add all nodes
  int nodes_added = 0;
  struct list_item *node_temp = graph->nodes;
  
  while (node_temp != NULL) {
    struct node *node_data = (struct node*)node_temp->data;
    printf("DEBUG: Adding node %d to main list\n", node_data->id);
    
    struct list_item* new_node = (struct list_item*)malloc(sizeof(struct list_item));
    if (new_node == NULL) {
      fprintf(stderr, "Error: Memory allocation failed in update_lists\n");
      return false;
    }
    new_node->data = node_temp->data;
    new_node->next = *node_list;
    *node_list = new_node;
    node_temp = node_temp->next;
    nodes_added++;
  }
  
  // Then add all lines
  int lines_added = 0;
  struct list_item *line_temp = graph->lines;
  
  while (line_temp != NULL) {
    struct line *line_data = (struct line*)line_temp->data;
    printf("DEBUG: Adding line (start=%d, end=%d) to main list\n", 
           line_data->startNode.id, line_data->endNode.id);
    
    struct list_item* new_line = (struct list_item*)malloc(sizeof(struct list_item));
    if (new_line == NULL) {
      fprintf(stderr, "Error: Memory allocation failed in update_lists\n");
      return false;
    }
    new_line->data = line_temp->data;
    new_line->next = *line_list;
    *line_list = new_line;
    line_temp = line_temp->next;
    lines_added++;
  }
  
  printf("DEBUG: Added %d nodes and %d lines to main lists\n", nodes_added, lines_added);
  
  // Check main lists after update
  main_node_count = 0;
  main_line_count = 0;
  
  for (count_ptr = *node_list; count_ptr != NULL; count_ptr = count_ptr->next) {
    main_node_count++;
  }
  
  for (count_ptr = *line_list; count_ptr != NULL; count_ptr = count_ptr->next) {
    main_line_count++;
  }
  
  printf("DEBUG: Main lists after update: %d nodes and %d lines\n", 
         main_node_count, main_line_count);
  
  return true;
}

struct graph complete_graph_create(int num_nodes, int *nodeCount, Color nodeColors[], int nodeDiameter, Vector2 mousePosition) {
    struct graph complete_graph;
    complete_graph.nodes = NULL;
    complete_graph.lines = NULL;
    complete_graph.id = next_graph_id;

    printf("DEBUG: Creating complete graph with %d nodes\n", num_nodes);

    int colorIndex = 0;

    // Generate node positions
    Vector2 *nodePositions = n_vertices_from_centroid(mousePosition, num_nodes);
    if (nodePositions == NULL) {
        fprintf(stderr, "Error: Failed to generate node positions for complete graph\n");
        return complete_graph;  // Return empty graph
    }

    // Create nodes
    for (int i = 0; i < num_nodes; i++) {
      //create node and add to list
      struct node *new_node = node_create(*nodeCount, colorIndex,
                                          nodeColors, nodeDiameter,
                                          nodePositions[i]);
      if (new_node == NULL) {
          fprintf(stderr, "Error: Failed to create node for complete graph\n");
          free(nodePositions);
          // Free already created nodes
          struct list_item *temp = complete_graph.nodes;
          while (temp != NULL) {
              struct list_item *next = temp->next;
              free(temp->data);
              free(temp);
              temp = next;
          }
          return complete_graph;  // Return empty graph
      }
      
      printf("DEBUG: Created node %d at position (%f, %f)\n", 
             *nodeCount, nodePositions[i].x, nodePositions[i].y);
      
      new_node->graph_id = next_graph_id;
      complete_graph.nodes = add_to_list(complete_graph.nodes, new_node);
      
      //update variables
      colorIndex++;
      (*nodeCount)++;
    }
    
    // Log nodes just to verify
    int node_count = 0;
    struct list_item *node_ptr;
    for (node_ptr = complete_graph.nodes; node_ptr != NULL; node_ptr = node_ptr->next) {
        struct node *node = (struct node*)node_ptr->data;
        printf("DEBUG: Node in list: id=%d, position=(%f, %f)\n", 
               node->id, node->position.x, node->position.y);
        node_count++;
    }
    printf("DEBUG: Found %d nodes in the list\n", node_count);
    
    // Create remaining lines to make complete graph
    int expected_lines = (num_nodes * (num_nodes - 1)) / 2;
    int actual_lines = 0;
    
    printf("DEBUG: Creating %d lines for complete graph\n", expected_lines);
    
    struct list_item *temp1 = complete_graph.nodes;
    int outer_node_id = -1;
    while (temp1 != NULL && temp1->next != NULL) {
        struct node *startNode = temp1->data;
        outer_node_id = startNode->id;
        
        printf("DEBUG: Processing lines from node %d\n", outer_node_id);
        
        struct list_item *temp2 = temp1->next;
        while (temp2 != NULL) {
            struct node *endNode = temp2->data;
            int inner_node_id = endNode->id;
            
            printf("DEBUG: Creating line between nodes %d and %d\n", 
                   outer_node_id, inner_node_id);
            
            // Use the proper line_create function with error checking
            struct line *new_line = line_create(startNode, endNode);
            if (new_line == NULL) {
                fprintf(stderr, "Error: Failed to create line for complete graph between nodes %d and %d\n", 
                        outer_node_id, inner_node_id);
                // Continue with graph creation despite this error
                temp2 = temp2->next;
                continue;
            }
            
            // Verify line data
            printf("DEBUG: Line created startNode=%d, endNode=%d\n", 
                   new_line->startNode.id, new_line->endNode.id);
            
            complete_graph.lines = add_to_list(complete_graph.lines, new_line);
            actual_lines++;
            temp2 = temp2->next;
        }
        temp1 = temp1->next;
    }
    
    printf("DEBUG: Created %d out of %d expected lines\n", actual_lines, expected_lines);
    
    // Count and debug list lines 
    int line_count = 0;
    struct list_item *line_ptr;
    for (line_ptr = complete_graph.lines; line_ptr != NULL; line_ptr = line_ptr->next) {
        struct line *line = (struct line*)line_ptr->data;
        printf("DEBUG: Line in list: start=%d, end=%d\n", 
               line->startNode.id, line->endNode.id);
        line_count++;
    }
    printf("DEBUG: Found %d lines in the list\n", line_count);

    free(nodePositions);
    next_graph_id++;

    return complete_graph;
}

int add_nodes_to_graph(struct node *nodes)
{
  int graph_id;
  /* Check nodes to see if they already belong to a graph (non-zero).
     If one of them belong to a graph, change all the graph ids of the other nodes to that node's graph id.
     Return the graph id number. */

  return graph_id;
}
