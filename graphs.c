#include "raylib.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <glib.h>///Getting around this in Makefile
#include <assert.h>

#define MAXNODES 20

GList* list = NULL;
//------------------------------------------------------------------------------------
void logError (int line, char* message)
{
  printf("[ %d %s]\n", line, message);
}
#define LOG( message ) logError( __LINE__, message )

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
      LIST_ENTRY(nodes) nodes;
    };
      
    LIST_HEAD(nodehead, nodes);
    struct nodes *currentNode;
    struct nodehead head;
    LIST_INIT(&head);
    currentNode = malloc(sizeof (struct nodes));

    typedef struct line {
      Node startNode;
      Node endNode;
    } Line;

    struct lines {
      Line line;
      LIST_ENTRY(lines) lines;
    };


    /* struct line lines[MAXNODES]; */
    /* int lineIndex = 0; */
    struct lines *currentLine;

    /* struct node nodes[MAXNODES]; */
    int structCount = 1;
    Node selectedNode;
    Node newNode;
    
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
    //Line list


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

        //Different things to do when mouse is over a node
        for (n = LIST_FIRST(&head); n != NULL; LIST_NEXT(n, nodes)) {
          isMouseOverNode = CheckCollisionPointCircle(
              mousePosition, n->node.position, nodeDiameter / 1.5);

          if (isMouseOverNode && !(isLineStarted || isLineEnded)) {
            isSpaceFree = false;
            isNodeDrawn = false;
          }
          if (isMouseOverNode && isLineStarted && !isNodeLocked) {
            nodeStruct = n;
            isNodeLocked = true;
          } else if (isMouseOverNode && isLineEnded && isNodeLocked) {
            /* startNode = lines[lineIndex].startNode; */
            selectedNode = n->node;
          }


          for (n2 = LIST_FIRST(&head); n2 != NULL; LIST_NEXT(n2, nodes)) {
            if (!CheckCollisionCircles(n->node.position, nodeDiameter / 2.0,
                                       n2->node.position, nodeDiameter / 2.0)) {
              if (isMouseOverNode && isNodeSelected && !isNodeLocked) {
                // Lock node so when moving, selection is kept.
                selectedNode = n->node;
                nodeStruct = n;
                isNodeLocked = true;
                break;
              }
            }
          }
        }

        //Create Node
        if (isNodeDrawn && isSpaceFree) {

          newNode.id = structCount;
          newNode.color = nodeColors[colorIndex];
          newNode.diameter = nodeDiameter;
          newNode.position = mousePosition;
          currentNode->node = newNode;
          LIST_INSERT_HEAD(&head, currentNode, nodes);
          colorIndex++;
          structCount++;
          isNodeDrawn = false;
        }

<<<<<<< HEAD
=======
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

>>>>>>> parent of a49c82f (Moved a bit of code)
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
        
<<<<<<< HEAD
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
=======
        for (int i = 0; i < MAXNODES; i++) {
          for (int j = 0; j < MAXNODES; j++) {
            DrawCircleV(nodes[i].position, nodes[i].diameter,
                        nodes[i].color);
            DrawText(TextFormat("%d", i + 1), nodes[i].position.x - 20,
                     nodes[i].position.y - 15, 8, BLACK);
>>>>>>> parent of a49c82f (Moved a bit of code)
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
