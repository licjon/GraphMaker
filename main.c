#include "raylib.h"
#include <stdbool.h>
#include <stdio.h>
#include "my_graph.h"

#define MAXNODES 20

//------------------------------------------------------------------------------------

int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 450;
    InitWindow(screenWidth, screenHeight, "Graph Maker");

    Vector2 mousePosition = { -100.0f, -100.0f };

    struct line lines[MAXNODES];
    int lineIndex = 0;

    struct node nodes[MAXNODES];
    int structCount = 0;
    int nodeIndex = 0;
    int nodeDiameter = 10;
    bool isNodeDrawn = false;
    bool isNodeSelected = false;
    bool isNodeLocked = false;
    int selectedIndex;
    Color nodeColors[MAXNODES] = {DARKBLUE, RED,   DARKGREEN,  ORANGE,  PINK,
                                  PURPLE,   BLACK, BROWN,      GOLD,    MAROON,
                                  YELLOW,   GRAY,  MAGENTA,       LIME,    SKYBLUE,
                                  VIOLET,   BEIGE, DARKBROWN,  GREEN,   LIGHTGRAY};
    int colorIndex = 0;

    bool isLineStarted = false;
    bool isLineEnded = false;
    int numOfLines = 0;
    struct node startNode;

    SetTargetFPS(60);     
    //---------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        mousePosition = GetMousePosition();

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) isNodeDrawn = true;
        else if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) isNodeSelected = true;
        else if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            isNodeSelected = false;
            isNodeLocked = false;
          }
        else if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
          if (!isLineStarted && !isLineEnded) {
            isLineStarted = true;
            isLineEnded = false;
          }
          else {
            isLineStarted = false;
            isLineEnded = true;
          }
        }

        bool isSpaceFree = true;
        bool isMouseOverNode = false;

        for (int i = 0; i < MAXNODES; i++) {
          isMouseOverNode = CheckCollisionPointCircle(
              mousePosition, nodes[i].position, nodeDiameter / 1.5);

          if (isMouseOverNode && !(isLineStarted || isLineEnded)) {
            isSpaceFree = false;
            isNodeDrawn = false;
          }
          if (isMouseOverNode && isLineStarted && !isNodeLocked) {
            selectedIndex = i;
            isNodeLocked = true;
          } else if (isMouseOverNode && isLineEnded && isNodeLocked) {
            selectedIndex = i;
          }

          for (int j = 0; j < MAXNODES; j++) {
            if (!CheckCollisionCircles(nodes[i].position, nodeDiameter / 2.0,
                                       nodes[j].position, nodeDiameter / 2.0)) {
              if (isMouseOverNode && isNodeSelected && !isNodeLocked) {
                // Lock node so when moving, selection is kept.
                selectedIndex = i;
                isNodeLocked = true;
                break;
              }
            }
          }
        }

        //Create Node
        if (isNodeDrawn && isSpaceFree && structCount <= MAXNODES - 1) {
          nodes[nodeIndex] = create_node(structCount,
                                         colorIndex, nodeColors, nodeDiameter,
                                         mousePosition);
                    /* printf("%d coordinates: %f, %f \n", nodeIndex, */
          /*        nodes[nodeIndex].position.x, nodes[nodeIndex].position.y); */
          nodeIndex++;
          colorIndex++;
          structCount++;
          isNodeDrawn = false;
        }


        // LINES---------------------
        if (isNodeSelected) {
          for (int k = 0; k < numOfLines; ++k) {
            //Move lines too
            if (nodes[selectedIndex].id == lines[k].startNode.id)
              lines[k].startNode.position = nodes[selectedIndex].position;
            if (nodes[selectedIndex].id == lines[k].endNode.id)
              lines[k].endNode.position = nodes[selectedIndex].position;
          }
        }
        //----------------------------------------------------------------------------------
        
        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
        
        ClearBackground(RAYWHITE);
        
        //Move Node
        if (isNodeSelected) {
          nodes[selectedIndex].position = mousePosition;
          DrawCircleV(nodes[selectedIndex].position,
                      nodes[selectedIndex].diameter,
                      nodes[selectedIndex].color);
          DrawText(TextFormat("%d", selectedIndex + 1),
                   nodes[selectedIndex].position.x - 20,
                   nodes[selectedIndex].position.y - 15, 8, BLACK);
          
        }

        for (int i = 0; i < nodeIndex; i++) {
            DrawCircleV(nodes[i].position, nodes[i].diameter,
                        nodes[i].color);
            DrawText(TextFormat("%d", i + 1), nodes[i].position.x - 20,
                     nodes[i].position.y - 15, 8, BLACK);
        }

        //Draw line before it is connected to end node
        if (isLineStarted) {
          lines[lineIndex].startNode = nodes[selectedIndex];
          DrawLine(lines[lineIndex].startNode.position.x,
                   lines[lineIndex].startNode.position.y, mousePosition.x,
                   mousePosition.y, BLACK);
        } else if (isLineEnded) {
          lines[lineIndex].endNode = nodes[selectedIndex];
          DrawLine(lines[lineIndex].startNode.position.x,
                   lines[lineIndex].startNode.position.y,
                   lines[lineIndex].endNode.position.x,
                   lines[lineIndex].endNode.position.y, BLACK);
          lineIndex++;
          isLineStarted = false;
          isLineEnded = false;
          isNodeLocked = false;
          numOfLines++;
        }

        // draw lines
        for (int k = 0; k < numOfLines; k++) {
          DrawLine(lines[k].startNode.position.x, lines[k].startNode.position.y,
                   lines[k].endNode.position.x, lines[k].endNode.position.y,
                   BLACK);
        }

        /* DrawText("Graph Maker", 10, 10, 20, DARKGRAY); */

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow(); // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}