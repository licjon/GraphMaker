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
    InitWindow(screenWidth, screenHeight, "Graph Maker");

    Vector2 mousePosition = { -100.0f, -100.0f };

    /* struct line lines[MAXNODES]; */
    int lineIndex = 0;

    /* struct node nodes[MAXNODES]; */

    /* struct node startNode; */
    struct list_item *node_list = NULL;
    struct list_item *head_node = node_list;

    int structCount = 0;
    int nodeIndex = 0;
    int nodeDiameter = 10;
    bool isNodeDrawn = false;
    bool isNodeSelected = false;
    bool isNodeLocked = false;
    struct node *selectedNode;
    Color nodeColors[NUM_OF_COLORS] = {DARKBLUE, RED,   DARKGREEN,  ORANGE,  PINK,
                                  PURPLE,   BLACK, BROWN,      GOLD,    MAROON,
                                  YELLOW,   GRAY,  MAGENTA,       LIME,    SKYBLUE,
                                  VIOLET,   BEIGE, DARKBROWN,  GREEN,   LIGHTGRAY};
    int colorIndex = 0;

    bool isLineStarted = false;
    bool isLineEnded = false;
    int numOfLines = 0;

    SetTargetFPS(60);     
    //---------------------------------------------------------------------------------------

    if (head_node == NULL) 
      printf("NULL\n");
    else
      printf("Not NULL\n");

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

        struct list_item *place;
        struct list_item *place2;
        for (place = node_list; place != NULL; place = place->next) {
          isMouseOverNode = CheckCollisionPointCircle(
              mousePosition, ((struct node *)place->data)->position,
              nodeDiameter / 1.5);

          if (isMouseOverNode && !(isLineStarted || isLineEnded)) {
            isSpaceFree = false;
            isNodeDrawn = false;
          }
          if (isMouseOverNode && isLineStarted && !isNodeLocked) {
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
          /* struct list_item new_item; */
          /* new_item.data = malloc(sizeof(struct node)); */
          /* *(struct node*)head_node->data = startNode; */
          if (colorIndex >= NUM_OF_COLORS)
            colorIndex = 0;

          struct node *new_node = create_node(structCount, colorIndex,
                                              nodeColors, nodeDiameter,
                                              mousePosition);
          printf("%d\n", new_node->id);
          node_list = add_to_list(node_list, new_node);
          /* nodeIndex++; */
          colorIndex++;
          structCount++;
          isNodeDrawn = false;
        }
        /* if (node_list != NULL) */
        /*   printf("\n\nThere is a node\n\n"); */

        // LINES---------------------
        /* if (isNodeSelected) { */
        /*   for (int k = 0; k < numOfLines; ++k) { */
        /*     //Move lines too */
        /*     if (nodes[selectedIndex].id == lines[k].startNode.id) */
        /*       lines[k].startNode.position = nodes[selectedIndex].position; */
        /*     if (nodes[selectedIndex].id == lines[k].endNode.id) */
        /*       lines[k].endNode.position = nodes[selectedIndex].position; */
        /*   } */
        /* } */
        //----------------------------------------------------------------------------------
        
        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
        
        ClearBackground(RAYWHITE);
        
        //Move Node
        /* if (isNodeSelected) { */
        /*   nodes[selectedIndex].position = mousePosition; */
        /*   DrawCircleV(nodes[selectedIndex].position, */
        /*               nodes[selectedIndex].diameter, */
        /*               nodes[selectedIndex].color); */
        /*   DrawText(TextFormat("%d", selectedIndex + 1), */
        /*            nodes[selectedIndex].position.x - 20, */
        /*            nodes[selectedIndex].position.y - 15, 8, BLACK); */
          
        /* } */

        /* if (!is_empty(head_node)) { */
          struct list_item *place3;
          for (place3 = node_list; place3 != NULL; place3 = place3->next) {
            DrawCircleV(((struct node *)place3->data)->position,
                        ((struct node *)place3->data)->diameter,
                        ((struct node *)place3->data)->color);
            DrawText(TextFormat("%d", ((struct node *)place3->data)->id + 1),
                     ((struct node *)place3->data)->position.x - 20,
                     ((struct node *)place3->data)->position.y - 15, 8, BLACK);
          }
        /* } */

        //Draw line before it is connected to end node
        /* if (isLineStarted) { */
        /*   lines[lineIndex].startNode = nodes[selectedIndex]; */
        /*   DrawLine(lines[lineIndex].startNode.position.x, */
        /*            lines[lineIndex].startNode.position.y, mousePosition.x, */
        /*            mousePosition.y, BLACK); */
        /* } else if (isLineEnded) { */
        /*   lines[lineIndex].endNode = nodes[selectedIndex]; */
        /*   DrawLine(lines[lineIndex].startNode.position.x, */
        /*            lines[lineIndex].startNode.position.y, */
        /*            lines[lineIndex].endNode.position.x, */
        /*            lines[lineIndex].endNode.position.y, BLACK); */
        /*   lineIndex++; */
        /*   isLineStarted = false; */
        /*   isLineEnded = false; */
        /*   isNodeLocked = false; */
        /*   numOfLines++; */
        /* } */

        // draw lines
        /* for (int k = 0; k < numOfLines; k++) { */
        /*   DrawLine(lines[k].startNode.position.x, lines[k].startNode.position.y, */
        /*            lines[k].endNode.position.x, lines[k].endNode.position.y, */
        /*            BLACK); */
        /* } */

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
