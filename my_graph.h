#ifndef MY_GRAPH_H
#define MY_GRAPH_H

#include "raylib.h"

struct node {
  int id;
  Vector2 position;
  int diameter;
  Color color;
};

struct line {
  struct node startNode;
  struct node endNode;
};

struct node create_node(int structCount, int colorIndex, Color nodeColors[], int nodeDiameter, Vector2 mousePosition);

#endif
