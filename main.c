/* List version of Graph Maker 4/5*/
#include "raylib.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "my_graph.h"

#define NUM_OF_COLORS 20

//------------------------------------------------------------------------------------

int main(void)
{
  // Initialization
  //--------------------------------------------------------------------------------------
  const int screenWidth = 800;
  const int screenHeight = 450;
  SetTraceLogCallback(CustomLog);
  InitWindow(screenWidth, screenHeight, "Graph Maker");

  Vector2 mousePosition = { -100.0f, -100.0f };

  struct list_item *line_list = NULL;

  struct list_item *node_list = NULL;

  struct node *selectedNode = NULL;

  struct graph cyclic_graph;
  struct graph complete_graph;

  int nodeCount = 0;
  int nodeDiameter = 10;
  bool isNodeDrawn = false;
  bool isNodeSelected = false;
  bool isNodeLocked = false;

  Color nodeColors[NUM_OF_COLORS] = {DARKBLUE, RED,   DARKGREEN,  ORANGE,  PINK,
                                     PURPLE,   BLACK, BROWN,      GOLD,    MAROON,
                                     YELLOW,   GRAY,  MAGENTA,       LIME,    SKYBLUE,
                                     VIOLET,   BEIGE, DARKBROWN,  GREEN,   LIGHTGRAY};
  int colorIndex = 0;

  float selectionDiameter = nodeDiameter / 1.01;


  bool isLineStarted = false;
  bool isLineEnded = false;
  bool createLine = false;
  bool endLine = false;

  int key_pressed = -1;
  bool isCPressed = false;
  float cPressedFrame = 0.0;

  bool isKPressed = false;
  float kPressedFrame = 0.0;

  SetTargetFPS(60);     
  //---------------------------------------------------------------------------------------

  // Main game loop
  while (!WindowShouldClose())    // Detect window close button or ESC key
    {
      // Update
      //----------------------------------------------------------------------------------
      mousePosition = GetMousePosition();

      // Draw node with left click
      if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !isLineStarted) isNodeDrawn = true;
      // Move node with mouse left button
      else if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) isNodeSelected = true;
      else if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
        isNodeSelected = false;
        isNodeLocked = false;
      }
      // Make line with right click
      else if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
        if (!isLineStarted && !isLineEnded) {
          isLineStarted = true;
          isLineEnded = false;
          createLine = true;
        } else {
          isLineStarted = false;
          isLineEnded = true;
          endLine = true;
        }
      } else if (IsKeyPressed(KEY_C)) {
        isCPressed = true;
        cPressedFrame = GetFrameTime();
      } else if (isCPressed) {
        isNodeDrawn = false;
        key_pressed = GetKeyPressed();
        key_pressed -= 48;

        if (key_pressed >= 3 && key_pressed <= 9) {
          cyclic_graph = cyclic_graph_create(key_pressed,
                                             &nodeCount,
                                             nodeColors,
                                             nodeDiameter,
                                             mousePosition);
          update_lists(&cyclic_graph, &node_list, &line_list);
          isCPressed = false;
        }
      } else if (IsKeyDown(KEY_K)) {
        isKPressed = true;
        kPressedFrame = GetFrameTime();
      } else if (isKPressed) {
        isNodeDrawn = false;
        key_pressed = GetKeyPressed();
        key_pressed -= 48;

        if (key_pressed >= 2 && key_pressed <= 9) {
          complete_graph = complete_graph_create(key_pressed,
                                                 &nodeCount,
                                                 nodeColors,
                                                 nodeDiameter,
                                                 mousePosition);
          update_lists(&complete_graph, &node_list, &line_list);
          isKPressed = false;
        }
      } else if (key_pressed != KEY_C || key_pressed != KEY_K) {
        isCPressed = false;
        isKPressed = false;
      }


      bool isSpaceFree = true;
      bool isMouseOverNode = false;

      /* Check node collisions */
      struct list_item *place = NULL;
      struct list_item *place2 = NULL;
      for (place = node_list; place != NULL; place = place->next) {
        isMouseOverNode = CheckCollisionPointCircle(
                                                    mousePosition,
                                                    ((struct node *)place->data)->position,
                                                    selectionDiameter);

        if (isMouseOverNode && !(isLineStarted || isLineEnded)) {
          isSpaceFree = false;
          isNodeDrawn = false;
          selectedNode = place->data;
        }
        if (isMouseOverNode && isLineStarted && !isNodeLocked) {
          isSpaceFree = false; // just added; is it needed
          selectedNode = place->data;
          isNodeLocked = true;
        } else if (isMouseOverNode && isLineEnded && isNodeLocked) {
          selectedNode = place->data;
        }

        for (place2 = node_list; place2 != NULL; place2 = place2->next) {
          if (!CheckCollisionCircles(((struct node *)place->data)->position,
                                     nodeDiameter / 2.0,
                                     ((struct node *)place2->data)->position,
                                     nodeDiameter / 2.0)) {
            if (isMouseOverNode && isNodeSelected && !isNodeLocked) {
              // Lock node so when moving, selection is kept.
              selectedNode = place->data;
              isNodeLocked = true;
              break;
            }
          }
        }
      }

      /* Create Node */
      if (isNodeDrawn && isSpaceFree) {
        if (colorIndex >= NUM_OF_COLORS)
          colorIndex = 0;

        struct node *new_node = node_create(nodeCount, colorIndex,
                                            nodeColors, nodeDiameter,
                                            mousePosition);
        node_list = add_to_list(node_list, new_node);
        colorIndex++;
        nodeCount++;
        isNodeDrawn = false;
      }

      // LINES---------------------

      if (createLine) {
        struct line *new_line = start_line(selectedNode);
        line_list = add_to_list(line_list, new_line);
        createLine = false;
      }

      if (isLineEnded) {
        end_line(line_list, selectedNode);
        isLineEnded = false;
        isNodeLocked = false;
      }

      //Update line positions when moving node
      struct list_item *line_head = NULL;
      if (isNodeSelected) {
        for (line_head = line_list; line_head != NULL; line_head = line_head->next) {
          if (selectedNode->id == ((struct line *)line_head->data)->startNode.id)
            ((struct line *)line_head->data)->startNode.position = selectedNode->position;
          if (selectedNode->id == ((struct line *)line_head->data)->endNode.id)
            ((struct line *)line_head->data)->endNode.position = selectedNode->position;
        }
      }
      //----------------------------------------------------------------------------------
        
      // Draw
      //----------------------------------------------------------------------------------
      BeginDrawing();
        
      ClearBackground(RAYWHITE);
        
      // Draw Nodes
      struct list_item *place3 = NULL;
      for (place3 = node_list; place3 != NULL; place3 = place3->next) {
        if (isNodeSelected && !(isLineStarted || isLineEnded))
          if (selectedNode != NULL) {
            if (mousePosition.x > screenWidth) {
              selectedNode->position.x = screenWidth;
            } else if (mousePosition.x < 0) {
              selectedNode->position.x = 0;
            } else {
              selectedNode->position.x = mousePosition.x;
            }
            if (mousePosition.y > screenHeight) {
              selectedNode->position.y = screenHeight;
            } else if (mousePosition.y < 0) {
              selectedNode->position.y = 0;
            } else {
              selectedNode->position.y = mousePosition.y;
            }
            
          } 

        DrawCircleV(((struct node *)place3->data)->position,
                    ((struct node *)place3->data)->diameter,
                    ((struct node *)place3->data)->color);
        DrawText(TextFormat("%d", ((struct node *)place3->data)->id + 1),
                 ((struct node *)place3->data)->position.x - 20,
                 ((struct node *)place3->data)->position.y - 15, 8, BLACK);
      }

      if (isNodeSelected)
          DrawCircleLines(selectedNode->position.x,
                          selectedNode->position.y,
                          selectedNode->diameter + (selectedNode->diameter / 2.0),
                          selectedNode->color);

      //Draw line before it is connected to end node
      if (isLineStarted) {
        DrawLine(((struct line *)line_list->data)->startNode.position.x,
                 ((struct line *)line_list->data)->startNode.position.y,
                 mousePosition.x, mousePosition.y, BLACK);
      }

      // draw lines
      if (line_list != NULL) {
        struct list_item *line_head2 = NULL;
        for (line_head2 = line_list; line_head2 != NULL; line_head2 = line_head2->next) {
          DrawLine(((struct line *)line_head2->data)->startNode.position.x,
                   ((struct line *)line_head2->data)->startNode.position.y,
                   ((struct line *)line_head2->data)->endNode.position.x,
                   ((struct line *)line_head2->data)->endNode.position.y,
                   BLACK);
        }
      }

      /* if (line_list != NULL) */
      /*   DrawText(TextFormat("End node x pos: %f\nEnd node y pos: %f", ((struct line *)line_list->data)->endNode.position.x, ((struct line *)line_list->data)->endNode.position.y), 10, 10, 20, DARKGRAY); */

      /* DrawText(TextFormat("isLineStarted: %d", isLineStarted), 10, 50, 20, DARKGRAY); */

      EndDrawing();
      //----------------------------------------------------------------------------------
    }

  // De-Initialization
  //--------------------------------------------------------------------------------------
  CloseWindow(); // Close window and OpenGL context
  //--------------------------------------------------------------------------------------

  return 0;
}
