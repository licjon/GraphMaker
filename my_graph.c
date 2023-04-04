#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "my_graph.h"

struct node *create_node(int nodeCount, int colorIndex, Color nodeColors[],
                        int nodeDiameter, Vector2 mousePosition) {
  struct node *node = malloc(sizeof(struct node));
  node->id = nodeCount;
  node->color = nodeColors[colorIndex];
  node->diameter = nodeDiameter;
  node->position = mousePosition;

  return node;
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
