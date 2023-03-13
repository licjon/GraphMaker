#include "raylib.h"
#include <stdbool.h>

#define MAXNODES 20
#define MAXLINES MAXNODES * (MAXNODES - 1) / 2
//------------------------------------------------------------------------------------

int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 450;
    InitWindow(screenWidth, screenHeight, "Graph Maker");

    struct node {
      Vector2 position;
      int diameter;
      Color color;
    };

    struct line {
      struct node startPos;
      struct node endPos; 
    };

    struct line lines[MAXLINES];// = {{{0, 0}, {0, 0}}};
    int linesIndex = 0;
    struct node nodes[MAXNODES];
    int nodeIndex = 0;
    int nodeDiameter = 10;
    Vector2 mousePosition = { -100.0f, -100.0f };
    Color nodeColors[MAXNODES] = {DARKBLUE, RED,   DARKGREEN,  ORANGE,  PINK,
                                  PURPLE,   BLACK, BROWN,      GOLD,    MAROON,
                                  YELLOW,   GRAY,  GOLD,       LIME,    SKYBLUE,
                                  VIOLET,   BEIGE, DARKBROWN,  GREEN,   LIGHTGRAY};
    int colorIndex = 0;
    bool isLMousePressed = false;
    bool isNodeSelected = false;
    bool isNodeLocked = false;
    bool isLineStarted = false;
    bool isLineEnded = false;
    bool firstRClick = false;
    bool secondRClick = false;

    int selectedIndex;
    SetTargetFPS(60);     
    //---------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
      // Update
      //----------------------------------------------------------------------------------
      mousePosition = GetMousePosition();
      
      if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) isLMousePressed = true;
      else if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) isNodeSelected = true;
      else if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
        isNodeSelected = false;
        isNodeLocked = false;
      }
      else if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
        if (!firstRClick) {
          firstRClick = true;
          secondRClick = false;
        } else if (firstRClick) {
          firstRClick = false;
          secondRClick = true;
        }
        //----------------------------------------------------------------------------------
        
        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
        
        ClearBackground(RAYWHITE);
        
        bool isSpaceFree = true;
        bool isMouseOverNode = false;
        //Check nodes for collisions and draw them
        for (int i = 0; i < MAXNODES; i++) {
          isMouseOverNode = CheckCollisionPointCircle(mousePosition, nodes[i].position, nodeDiameter/2.0);
          if (isMouseOverNode && !firstRClick && !secondRClick) {
            // If node is clicked, do not create node on top of it.
            isSpaceFree = false; 
            selectedIndex = i; //Remember index so that the line will start here.
          }
          if (isMouseOverNode && firstRClick) {
            isLineStarted = true;
            selectedIndex = 1;
          }
          if (isMouseOverNode && secondRClick) {
            isLineEnded = true;
            selectedIndex = i;
          }
          for (int j = 0; j < MAXNODES; j++) {
            if (!CheckCollisionCircles(nodes[i].position, nodeDiameter/2.0, nodes[j].position, nodeDiameter/2.0)) {
              if (CheckCollisionPointCircle(mousePosition, nodes[i].position, nodeDiameter/2.0) && isNodeSelected && !isNodeLocked) { 
                // Select only one node until left mouse button is released. Only move the node that is selected even when moving over another node. 
                selectedIndex = i;
                isNodeLocked = true;
                break;
              }
              else {
                // Draw all the nodes
                DrawCircleV(nodes[i].position, nodes[i].diameter, nodes[i].color);
                DrawText(TextFormat("%d", i + 1), nodes[i].position.x - 20, nodes[i].position.y - 15, 8, BLACK);
              }
            }
          }
        }

        // Draw lines
        for (int k = 0; k < MAXLINES; k++) {
          DrawLine(lines[k].startPos.position.x, lines[k].startPos.position.y,
                   lines[k].endPos.position.x, lines[k].endPos.position.y,
                   BLACK);
        }

        if (isLMousePressed && isSpaceFree) {
          // Draw a new node
          nodes[nodeIndex].color = nodeColors[colorIndex];
          nodes[nodeIndex].diameter = nodeDiameter;
          nodes[nodeIndex].position = mousePosition;
          nodeIndex++;
          colorIndex++;
          isLMousePressed = false;
        } else if (isSpaceFree && secondRClick) {
          // If making a line and clicking on blank area, stop making line.
          isLineStarted = false;
        } else if (firstRClick) {
          // Start a new line
          lines[linesIndex].startPos.position = nodes[selectedIndex].position;
          lines[linesIndex].endPos.position = mousePosition;
        } else if (secondRClick) {
          // End line
          lines[linesIndex].endPos.position = nodes[selectedIndex].position;
          /* lines[linesIndex + 1].startPos.position =
           * nodes[selectedIndex].position; */
          /* lines[linesIndex + 1].endPos.position = mousePosition; */

          secondRClick = false;
          linesIndex++;
        }

        // Move selected node
        if (isNodeSelected) {
          nodes[selectedIndex].position = mousePosition;
          DrawCircleV(nodes[selectedIndex].position,
                      nodes[selectedIndex].diameter,
                      nodes[selectedIndex].color);
          DrawText(TextFormat("%d", selectedIndex + 1),
                   nodes[selectedIndex].position.x - 20,
                   nodes[selectedIndex].position.y - 15, 8, BLACK);
        }

        DrawText("Graph Maker", 10, 10, 20, DARKGRAY);

        EndDrawing();
        //----------------------------------------------------------------------------------
      }

      // De-Initialization
      //--------------------------------------------------------------------------------------
      CloseWindow(); // Close window and OpenGL context
      //--------------------------------------------------------------------------------------
      
      return 0;
    }
}
