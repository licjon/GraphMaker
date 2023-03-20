#include "my_graph.h"


struct node create_node(int structCount, int colorIndex, Color nodeColors[],
                        int nodeDiameter, Vector2 mousePosition) {
  struct node node;
  node.id = structCount;
  node.color = nodeColors[colorIndex];
  node.diameter = nodeDiameter;
  node.position = mousePosition;

  return node;
}


