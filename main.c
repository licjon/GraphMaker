/* List version of Graph Maker 4/5*/
#include "raylib.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "my_graph.h"

#define NUM_OF_COLORS 20

//------------------------------------------------------------------------------------
int GetDisplayWidth()
{
  if (IsWindowFullscreen())
    {
      int monitor = GetCurrentMonitor();
      return GetMonitorWidth(monitor);
    }
  else{
    return GetScreenWidth();
  }
}

int GetDisplayHeight()
{
  if (IsWindowFullscreen())
    {
      int monitor = GetCurrentMonitor();
      return GetMonitorHeight(monitor);
    }
  else{
    return GetScreenHeight();
  }
}

void ToggleFullScreenWindow(int windowWidth, int windowHeight)
{
  if (!IsWindowFullscreen())
    {
      int monitor = GetCurrentMonitor();
      SetWindowSize(GetDisplayWidth(), GetDisplayHeight());
      ToggleFullscreen();
    }
  else{
    ToggleFullscreen();
    SetWindowSize(windowWidth, windowHeight);
  }
}



int main(void)
{
  // Initialization
  //--------------------------------------------------------------------------------------
  const int screenWidth = 2560;
  const int screenHeight = 1440;
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
      } else if (IsKeyPressed(KEY_K)) {
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
      if (IsKeyPressed(KEY_SPACE)) {
        ToggleFullScreenWindow(screenWidth, screenHeight);
      }


      bool isSpaceFree = true;
      bool isMouseOverNode = false;

      /* Check node collisions */
      struct list_item *currentNode = NULL;
      struct list_item *currentNode2 = NULL;
      for (currentNode = node_list; currentNode != NULL; currentNode = currentNode->next) {
        isMouseOverNode = CheckCollisionPointCircle(
                                                    mousePosition,
                                                    ((struct node *)currentNode->data)->position,
                                                    selectionDiameter);

        if (isMouseOverNode && !(isLineStarted || isLineEnded) && !isNodeLocked) {
          /* Then create node */
          isSpaceFree = false;
          isNodeDrawn = false;
          selectedNode = currentNode->data;
        }
        if (isMouseOverNode && isLineStarted && !isNodeLocked) {
          /* Then end line */
          isSpaceFree = false;
          isNodeLocked = true;
          selectedNode = currentNode->data;
        } else if (isMouseOverNode && isLineEnded && isNodeLocked) {
          /* Complete line */
          selectedNode = currentNode->data;
        }

        for (currentNode2 = node_list; currentNode2 != NULL; currentNode2 = currentNode2->next) {
          if (!CheckCollisionCircles(((struct node *)currentNode->data)->position,
                                     nodeDiameter / 2.0,
                                     ((struct node *)currentNode2->data)->position,
                                     nodeDiameter / 2.0)) {
            if (isMouseOverNode && isNodeSelected && !isNodeLocked) {
              /* Lock node so when moving, selection is kept. */
              selectedNode = currentNode->data;
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
      struct list_item *currentNode3 = NULL;
      for (currentNode3 = node_list; currentNode3 != NULL; currentNode3 = currentNode3->next) {
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

        DrawCircleV(((struct node *)currentNode3->data)->position,
                    ((struct node *)currentNode3->data)->diameter,
                    ((struct node *)currentNode3->data)->color);
        DrawText(TextFormat("%d", ((struct node *)currentNode3->data)->id + 1),
                 ((struct node *)currentNode3->data)->position.x - 20,
                 ((struct node *)currentNode3->data)->position.y - 15, 8, BLACK);
      }

      //Draw outline around selected node
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
