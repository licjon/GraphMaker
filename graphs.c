#include "raylib.h"
#include <stdbool.h>
#include <stdio.h>
#include <sys/queue.h>

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

    typedef struct node {
      int id;
      Vector2 position;
      int diameter;
      Color color;
    } Node;

    struct nodes {
      Node node;
      STAILQ_ENTRY(nodes) nodes;
    };
      
    typedef struct line {
      Node startNode;
      Node endNode;
    } Line;

    struct lines {
      Line line;
      STAILQ_ENTRY(lines) lines;
    };

    /* struct line lines[MAXNODES]; */
    /* int lineIndex = 0; */
    struct lines *currentLine;

    /* struct node nodes[MAXNODES]; */
    int structCount = 1;
    Node selectedNode;
    /* int nodeIndex = 0; */
    int nodeDiameter = 10;
    bool isNodeDrawn = false;
    bool isNodeSelected = false;
    bool isNodeLocked = false;
    /* int selectedIndex; */
    Color nodeColors[MAXNODES] = {DARKBLUE, RED,   DARKGREEN,  ORANGE,  PINK,
                                  PURPLE,   BLACK, BROWN,      GOLD,    MAROON,
                                  YELLOW,   GRAY,  MAGENTA,       LIME,    SKYBLUE,
                                  VIOLET,   BEIGE, DARKBROWN,  GREEN,   LIGHTGRAY};
    int colorIndex = 0;

    bool isLineStarted = false;
    bool isLineEnded = false;
    int numOfLines = 0;

    //Node list
    STAILQ_HEAD(nodehead, nodes) head;
    STAILQ_INIT(&head);

    //Line list
    STAILQ_HEAD(linehead, lines) lhead;
    STAILQ_INIT(&lhead);
    currentLine = STAILQ_FIRST(&lhead);

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

        struct nodes *n, *n2, *nodeStruct;
        for (n = STAILQ_FIRST(&head); n != NULL; STAILQ_NEXT(n, nodes)) {
          isMouseOverNode = CheckCollisionPointCircle(
              mousePosition, n->node.position, nodeDiameter / 1.5);

          if (isMouseOverNode && !(isLineStarted || isLineEnded)) {
            isSpaceFree = false;
            isNodeDrawn = false;
          }
          if (isMouseOverNode && isLineStarted && !isNodeLocked) {
            selectedNode = n->node;
            nodeStruct = n;
            isNodeLocked = true;
          } else if (isMouseOverNode && isLineEnded && isNodeLocked) {
            /* startNode = lines[lineIndex].startNode; */
            selectedNode = n->node;
          }


          for (n2 = STAILQ_FIRST(&head); n2 != NULL; STAILQ_NEXT(n2, nodes)) {
            if (!CheckCollisionCircles(n2->node.position, nodeDiameter / 2.0,
                                       n2->node.position, nodeDiameter / 2.0)) {
              if (isMouseOverNode && isNodeSelected && !isNodeLocked) {
                // Lock node so when moving, selection is kept.
                selectedNode = n2->node;
                nodeStruct = n2;
                isNodeLocked = true;
                break;
              }
            }
          }
        }

        //Create Node
        if (isNodeDrawn && isSpaceFree) {
          selectedNode.id = structCount;
          selectedNode.color = nodeColors[colorIndex];
          selectedNode.diameter = nodeDiameter;
          selectedNode.position = mousePosition;
          STAILQ_NEXT(nodeStruct, nodes);
          colorIndex++;
          isNodeDrawn = false;
        }

        // LINES---------------------

        struct lines *ln;
        if (isNodeSelected) {
          for (ln = STAILQ_FIRST(&lhead); ln != NULL; STAILQ_NEXT(ln, lines)) {
            //Move lines too
            if (selectedNode.id == ln->line.startNode.id)
              ln->line.startNode.position = selectedNode.position;
            if (selectedNode.id == ln->line.endNode.id)
              ln->line.endNode.position = selectedNode.position;
          }
        }
        //----------------------------------------------------------------------------------
        
        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
        
        ClearBackground(RAYWHITE);
        
        //Move Node
        if (isNodeSelected) {
          selectedNode.position = mousePosition;
          DrawCircleV(selectedNode.position,
                      selectedNode.diameter,
                      selectedNode.color);
          DrawText(TextFormat("%d", selectedNode.id),
                   selectedNode.position.x - 20,
                   selectedNode.position.y - 15, 8, BLACK);
        }

        
        struct nodes *n3;
        for (n3 = STAILQ_FIRST(&head); n3 != NULL; STAILQ_NEXT(n3, nodes)) {
            DrawCircleV(n3->node.position, n3->node.diameter,
                        n3->node.color);
            DrawText(TextFormat("%d", n3->node.id), n3->node.position.x - 20,
                     n3->node.position.y - 15, 8, BLACK);
          }

        //Draw line before it is connected to end node
        if (isLineStarted) {
          currentLine->line.startNode = selectedNode;
          DrawLine(currentLine->line.startNode.position.x,
                   currentLine->line.startNode.position.y, mousePosition.x,
                   mousePosition.y, BLACK);
        } else if (isLineEnded) {
          currentLine->line.endNode = selectedNode;
          DrawLine(currentLine->line.startNode.position.x,
                   currentLine->line.startNode.position.y,
                   currentLine->line.endNode.position.x,
                   currentLine->line.endNode.position.y, BLACK);
          STAILQ_NEXT(currentLine, lines);
          isLineStarted = false;
          isLineEnded = false;
          isNodeLocked = false;
          numOfLines++;
        }

        // draw lines
        
        struct lines *ln2;
          for (ln2 = STAILQ_FIRST(&lhead); ln2 != NULL; STAILQ_NEXT(ln2, lines)) {
          DrawLine(ln2->line.startNode.position.x, ln2->line.startNode.position.y,
                   ln2->line.endNode.position.x, ln2->line.endNode.position.y,
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
