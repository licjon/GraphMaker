/* List version of Graph Maker 4/5*/
#include "raylib.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "my_graph.h"

#define NUM_OF_COLORS 20
#define FAIL_EXIT(msg) do { fprintf(stderr, "ERROR: %s\n", msg); exit(EXIT_FAILURE); } while(0)

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

void cleanup_resources(struct list_item **node_list, struct list_item **line_list) {
    // Free all nodes
    struct list_item *current = *node_list;
    struct list_item *next;
    
    while (current != NULL) {
        next = current->next;
        free(current->data);  // Free the node data
        free(current);        // Free the list item
        current = next;
    }
    *node_list = NULL;
    
    // Free all lines
    current = *line_list;
    while (current != NULL) {
        next = current->next;
        free(current->data);  // Free the line data
        free(current);        // Free the list item
        current = next;
    }
    *line_list = NULL;
}

// New error handling function
void handle_error(const char* msg, struct list_item **node_list, struct list_item **line_list) {
    fprintf(stderr, "ERROR: %s\n", msg);
    cleanup_resources(node_list, line_list);
    CloseWindow();
    exit(EXIT_FAILURE);
}

// Global pointers for signal handler
struct list_item *g_node_list = NULL;
struct list_item *g_line_list = NULL;

// Signal handler for cleanup on unexpected termination
void signal_handler(int signal) {
    fprintf(stderr, "Caught signal %d, cleaning up resources...\n", signal);
    cleanup_resources(&g_node_list, &g_line_list);
    CloseWindow();
    exit(EXIT_FAILURE);
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

  // Set up global pointers for signal handler
  g_node_list = node_list;
  g_line_list = line_list;

  // Set up signal handlers
  signal(SIGINT, signal_handler);
  signal(SIGTERM, signal_handler);
  signal(SIGSEGV, signal_handler);

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

  bool isSelecting = false;
  bool isMultiSelect = false;

  Vector2 startAreaPosition = {0};
  Vector2 endAreaPosition = {0};
  Rectangle selectionRect = {0};
  Color backgroundColor = RAYWHITE;
  
  // Used for multi-select dragging
  static bool firstDragFrame = true;
  static Vector2 initialMousePosForDrag = {0}; // Shared mouse position for multi-select drag
  
  // Used to prevent re-selection after mouse release
  static bool preventSelectionCooldown = false;
  static int cooldownFrames = 0;
  static int COOLDOWN_DURATION = 5; // Increased cooldown frames
  
  // Used to track whether a node was just created
  static bool nodeJustCreated = false;
  
  SetTargetFPS(60);     
  //---------------------------------------------------------------------------------------

  // Main game loop
  while (!WindowShouldClose())    // Detect window close button or ESC key
    {
      // Update
      //----------------------------------------------------------------------------------
      mousePosition = GetMousePosition();

      // Handle cooldown after mouse release
      if (preventSelectionCooldown) {
        cooldownFrames++;
        if (cooldownFrames > COOLDOWN_DURATION) { // Longer cooldown
          preventSelectionCooldown = false;
          cooldownFrames = 0;
          printf("Cooldown ended, selection enabled again\n");
        }
      }

      /* Area Selection */
      if (IsMouseButtonDown(MOUSE_BUTTON_MIDDLE)) {
        if (!isSelecting) {
          startAreaPosition = mousePosition;
          isSelecting = true;
          
          // Clear any existing selection when starting a new selection area
          if (isMultiSelect) {
            struct list_item *currentNode = NULL;
            for (currentNode = node_list; currentNode != NULL; currentNode = currentNode->next) {
              struct node *currentNodeData = (struct node *)currentNode->data;
              currentNodeData->selected = false;
            }
          }
        }

        endAreaPosition = mousePosition;
        float width = endAreaPosition.x - startAreaPosition.x;
        float height = endAreaPosition.y - startAreaPosition.y;
        
        if (width < 0) {
          selectionRect.x = endAreaPosition.x;
          selectionRect.width = -width;
        } else {
          selectionRect.x = startAreaPosition.x;
          selectionRect.width = width;
        }
        if (height < 0) {
          selectionRect.y = endAreaPosition.y;
          selectionRect.height = -height;
        } else {
          selectionRect.y = startAreaPosition.y;
          selectionRect.height = height;
        }
      } else if (isSelecting) {
        isSelecting = false;
        isMultiSelect = true;
        
        // Select all nodes in the area
        struct list_item *currentNode = NULL;
        int nodesSelected = 0; // Track how many nodes are selected
        
        for (currentNode = node_list; currentNode != NULL; currentNode = currentNode->next) {
          struct node *currentNodeData = (struct node *)currentNode->data;
          if ((currentNodeData->position.x > selectionRect.x) && 
              (currentNodeData->position.x < (selectionRect.x + selectionRect.width)) &&
              (currentNodeData->position.y > selectionRect.y) && 
              (currentNodeData->position.y < (selectionRect.y + selectionRect.height))) {
            currentNodeData->selected = true;
            nodesSelected++;
            
            // Save original position of each selected node
            currentNodeData->originalPosition = currentNodeData->position;
          } else if (isMultiSelect) {
            // Only deselect nodes if we're not extending a selection with a modifier key
            currentNodeData->selected = false;
          }
        }
        
        // If no nodes were selected, exit multi-select mode
        if (nodesSelected == 0) {
          isMultiSelect = false;
        }
        
        printf("Multi-select activated with %d nodes selected\n", nodesSelected);
      }
           
      // Exit multi-select mode when clicking in empty space
      else if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !isLineStarted && isMultiSelect && !preventSelectionCooldown) {
        // Check if clicking on any node
        bool clickedOnNode = false;
        struct list_item *currentNode = NULL;
        for (currentNode = node_list; currentNode != NULL; currentNode = currentNode->next) {
          struct node *currentNodeData = (struct node *)currentNode->data;
          if (CheckCollisionPointCircle(mousePosition, currentNodeData->position, selectionDiameter)) {
            clickedOnNode = true;
            // Store the clicked node for movement but DO NOT deselect other nodes
            selectedNode = currentNodeData;
            
            // Make sure this node is also selected
            currentNodeData->selected = true;
            
            printf("Clicked on node %d in multi-select mode\n", currentNodeData->id);
            break;
          }
        }
        
        // If clicked outside any node, exit multi-select mode
        if (!clickedOnNode) {
          printf("Clicked in empty space, exiting multi-select mode\n");
          isMultiSelect = false;
          // Deselect all nodes
          for (currentNode = node_list; currentNode != NULL; currentNode = currentNode->next) {
            struct node *currentNodeData = (struct node *)currentNode->data;
            currentNodeData->selected = false;
          }
        }
      }
      
      // Draw node with left click - but not during cooldown period
      else if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !isLineStarted) {
        // Only set isNodeDrawn to true if we're not over an existing node
        bool isOverExistingNode = false;
        struct list_item *checkNode = NULL;
        for (checkNode = node_list; checkNode != NULL; checkNode = checkNode->next) {
          if (CheckCollisionPointCircle(mousePosition, 
                                       ((struct node *)checkNode->data)->position,
                                       selectionDiameter)) {
            isOverExistingNode = true;
            break;
          }
        }
        
        if (!isOverExistingNode) {
          printf("Setting isNodeDrawn=true for mouse at (%f, %f)\n", mousePosition.x, mousePosition.y);
          isNodeDrawn = true;
        }
      }

      // Store original positions when starting a drag operation
      if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && isMultiSelect) {
        printf("Starting multi-select drag operation\n");
        
        // Store original positions of all selected nodes
        struct list_item *currentNode = NULL;
        for (currentNode = node_list; currentNode != NULL; currentNode = currentNode->next) {
          struct node *currentNodeData = (struct node *)currentNode->data;
          if (currentNodeData->selected) {
            currentNodeData->originalPosition = currentNodeData->position;
          }
        }
        
        // Store starting mouse position
        startAreaPosition = mousePosition;
        initialMousePosForDrag = mousePosition;
        firstDragFrame = true;
        
        // Clear the selection rectangle so movement isn't constrained
        selectionRect.x = 0;
        selectionRect.y = 0;
        selectionRect.width = 0;
        selectionRect.height = 0;
      }

      // Move node with mouse left button - but not during cooldown
      else if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && !preventSelectionCooldown) {
        isNodeSelected = true;
      }
      else if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
        printf("Mouse released, resetting all selection variables\n");
        isNodeSelected = false;
        isNodeLocked = false;
        isNodeDrawn = false;
        nodeJustCreated = false;

        // Reset the drag tracking variable
        firstDragFrame = true;
        
        // Enable a longer cooldown to prevent immediate re-selection
        preventSelectionCooldown = true;
        cooldownFrames = 0;
        
        // Exit multi-select mode immediately when releasing mouse
        if (isMultiSelect) {
          printf("Mouse released, exiting multi-select mode\n");
          isMultiSelect = false;
          selectedNode = NULL;
          
          // Deselect all nodes
          struct list_item *currentNode = NULL;
          for (currentNode = node_list; currentNode != NULL; currentNode = currentNode->next) {
            struct node *currentNodeData = (struct node *)currentNode->data;
            currentNodeData->selected = false;
            currentNodeData->locked = false;
          }
        } else {
          // If not in multi-select mode, deselect all nodes as usual
          struct list_item *currentNode = NULL;
          for (currentNode = node_list; currentNode != NULL; currentNode = currentNode->next) {
            struct node *currentNodeData = (struct node *)currentNode->data;
            currentNodeData->selected = false;
            currentNodeData->locked = false;
          }
        }
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

        /* Make Cyclic Graph */
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
          if (cyclic_graph.nodes == NULL) {
            fprintf(stderr, "Warning: Failed to create cyclic graph\n");
          } else {
            bool update_success = update_lists(&cyclic_graph, &node_list, &line_list);
            if (!update_success) {
              fprintf(stderr, "Warning: Failed to update lists with cyclic graph\n");
              handle_error("Failed to update lists with cyclic graph", &node_list, &line_list);
            }
          }
          isCPressed = false;
        }

        /* Make Complete Graph */
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
          if (complete_graph.nodes == NULL) {
            fprintf(stderr, "Warning: Failed to create complete graph\n");
          } else {
            bool update_success = update_lists(&complete_graph, &node_list, &line_list);
            if (!update_success) {
              fprintf(stderr, "Warning: Failed to update lists with complete graph\n");
              handle_error("Failed to update lists with complete graph", &node_list, &line_list);
            }
          }
          isKPressed = false;
        }
      } else if (key_pressed != KEY_C || key_pressed != KEY_K) {
        isCPressed = false;
        isKPressed = false;
      }

      /* Toggle full screen */
      if (IsKeyPressed(KEY_SPACE)) {
        ToggleFullScreenWindow(screenWidth, screenHeight);
      }
      
      /* Check node collisions */
      bool isSpaceFree = true;
      bool isMouseOverNode = false;
      struct list_item *currentNode = NULL;
      struct list_item *currentNode2 = NULL;

      // Skip node collision checks if:
      // 1. In multi-select mode
      // 2. During cooldown period
      // 3. Just created a node
      if (!isMultiSelect && !preventSelectionCooldown && !nodeJustCreated) {
        for (currentNode = node_list; currentNode != NULL; currentNode = currentNode->next) {
          struct node *currentNodeData = (struct node *)currentNode->data;

          isMouseOverNode = CheckCollisionPointCircle(
                                                    mousePosition,
                                                    (currentNodeData)->position,
                                                    selectionDiameter);

          // Only handle node selection if we're not in multi-select mode
          if (isMouseOverNode && !(isLineStarted || isLineEnded) && !isNodeLocked) {
            /* Then create node */
            isSpaceFree = false;
            isNodeDrawn = false;  // Important - prevent node creation when over existing node
            selectedNode = currentNode->data; 
            
            // When not in multi-select mode, clicking a node selects only that node
            struct list_item *tempNode = NULL;
            for (tempNode = node_list; tempNode != NULL; tempNode = tempNode->next) {
              ((struct node *)tempNode->data)->selected = false;
            }
            // Select the clicked node
            currentNodeData->selected = true;
            
            printf("Selected node %d at (%f, %f)\n", currentNodeData->id, 
                   currentNodeData->position.x, currentNodeData->position.y);
          } 
          else if (isMouseOverNode && isLineStarted && !isNodeLocked) {
            /* Then end line */
            isSpaceFree = false;
            /* currentNodeData->locked = true; */
            isNodeLocked = true;
            selectedNode = currentNode->data;
            /* currentNodeData->selected = true; */
          } else if (isMouseOverNode && isLineEnded && isNodeLocked) {
            /* Complete line */
            selectedNode = currentNode->data;
            /* currentNodeData->selected = true; */
          }
            /* Area selection handled separately */
          
          for (currentNode2 = node_list; currentNode2 != NULL; currentNode2 = currentNode2->next) {
            if (!CheckCollisionCircles(currentNodeData->position,
                                       nodeDiameter / 2.0,
                                       ((struct node *)currentNode2->data)->position,
                                       nodeDiameter / 2.0)) {
              if (isMouseOverNode && isNodeSelected && !isNodeLocked) {
                /* Lock node so when moving, selection is kept. */
                selectedNode = currentNode->data;
                /* currentNodeData->selected = true; */
                /* currentNodeData->locked = true; */
                isNodeLocked = true;
                break;
              }
            }
          }
        }
      }

      /* Create Node */
      if (isNodeDrawn && isSpaceFree) {
        if (colorIndex >= NUM_OF_COLORS)
          colorIndex = 0;

        // Ensure we're creating a new node, not moving an existing one
        printf("Creating new node at (%f, %f), selectedNode=%p\n", mousePosition.x, mousePosition.y, selectedNode);
        
        // CRITICAL: Set selectedNode to NULL before creating a new node
        selectedNode = NULL;
        nodeJustCreated = true;
        
        // Reset node selection flags to prevent segfault
        isNodeSelected = false;
        isNodeLocked = false;
        
        struct node *new_node = node_create(nodeCount, colorIndex,
                                            nodeColors, nodeDiameter,
                                            mousePosition);
        if (new_node == NULL) {
          handle_error("Failed to create new node", &node_list, &line_list);
        }
        
        struct list_item *new_list = add_to_list(node_list, new_node);
        if (new_list == NULL) {
          free(new_node); // Clean up allocated node if list addition fails
          handle_error("Failed to add node to list", &node_list, &line_list);
        }
        node_list = new_list;
        // Update global pointer for signal handler
        g_node_list = node_list;
        colorIndex++;
        nodeCount++;
        isNodeDrawn = false;
      }

      // LINES---------------------
      struct node start_node;
      if (createLine) {
        if (selectedNode == NULL) {
          fprintf(stderr, "Warning: Trying to create line with NULL node\n");
          createLine = false;
        } else {
          struct line *new_line = start_line(selectedNode);
          if (new_line == NULL) {
            handle_error("Failed to create new line", &node_list, &line_list);
          }
          
          struct list_item *new_list = add_to_list(line_list, new_line);
          if (new_list == NULL) {
            free(new_line); // Clean up allocated line if list addition fails
            handle_error("Failed to add line to list", &node_list, &line_list);
          }
          line_list = new_list;
          // Update global pointer for signal handler
          g_line_list = line_list;
          createLine = false;
        }
      }

      struct node end_node;
      if (isLineEnded) {
        if (selectedNode == NULL || line_list == NULL) {
          fprintf(stderr, "Warning: Cannot end line with NULL node or empty line list\n");
        } else {
          end_line(line_list, selectedNode);
        }
        isLineEnded = false;
        isNodeLocked = false;
      }

      //Update line positions when moving node
      struct list_item *line_head = NULL;
      if (isNodeSelected && selectedNode != NULL) {
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
        
      ClearBackground(backgroundColor);
      
      // For multi-select, calculate movement constraints to maintain shape
      if (isMultiSelect && isNodeSelected) {
        // If it's the first frame of dragging, initialize was already done when mouse was pressed
        if (firstDragFrame) {
          firstDragFrame = false;
          printf("First frame of multi-select drag, using saved positions\n");
        }
        
        float deltaX = mousePosition.x - initialMousePosForDrag.x;
        float deltaY = mousePosition.y - initialMousePosForDrag.y;
        
        // Find leftmost, rightmost, topmost, and bottommost nodes in selection
        float leftmostX = screenWidth;
        float rightmostX = 0;
        float topmostY = screenHeight;
        float bottommostY = 0;
        
        // First, calculate the bounds of the selected nodes group
        struct list_item *boundNode = NULL;
        for (boundNode = node_list; boundNode != NULL; boundNode = boundNode->next) {
          struct node *nodeData = (struct node *)boundNode->data;
          if (nodeData->selected) {
            // Calculate where this node would be after applying the movement delta
            float potentialX = nodeData->originalPosition.x + deltaX;
            float potentialY = nodeData->originalPosition.y + deltaY;
            
            // Update the extremes
            if (potentialX < leftmostX) leftmostX = potentialX;
            if (potentialX > rightmostX) rightmostX = potentialX;
            if (potentialY < topmostY) topmostY = potentialY;
            if (potentialY > bottommostY) bottommostY = potentialY;
          }
        }
        
        // Calculate adjustments needed to keep all nodes within screen
        float adjustX = 0;
        float adjustY = 0;
        
        // Check if any node would go out of bounds and calculate adjustment
        if (leftmostX < 0) adjustX = -leftmostX;
        if (rightmostX > screenWidth) adjustX = screenWidth - rightmostX;
        if (topmostY < 0) adjustY = -topmostY;
        if (bottommostY > screenHeight) adjustY = screenHeight - bottommostY;
        
        // Adjust the mouse position to apply the corrected movement
        // This effectively maintains the shape by moving all nodes together
        mousePosition.x += adjustX;
        mousePosition.y += adjustY;
        
        // Debug output
        if (adjustX != 0 || adjustY != 0) {
          printf("Group boundary hit: adjusting by (%f, %f)\n", adjustX, adjustY);
        }
      }
        
      // Draw Nodes
      struct list_item *currentNode3 = NULL;
      for (currentNode3 = node_list; currentNode3 != NULL; currentNode3 = currentNode3->next)
        {
          struct node *currentNode3Data = (struct node *)currentNode3->data;

          /* Individual selected node position */
          if (isNodeSelected && !(isLineStarted || isLineEnded) && !nodeJustCreated)
            if (selectedNode != NULL) {
              // Make sure this node is actually selected (additional safety check)
              if (!selectedNode->selected) {
                printf("WARNING: selectedNode %d is not marked as selected!\n", selectedNode->id);
                selectedNode->selected = true;
              }
              
              printf("Moving node %d to (%f, %f)\n", selectedNode->id, mousePosition.x, mousePosition.y);
              
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
            } else {
              printf("WARNING: Trying to move node but selectedNode is NULL!\n");
            }

          /* Multi-select move - only executed for nodes that are selected */
          if (isMultiSelect && currentNode3Data->selected && isNodeSelected) {
            // Calculate the deltas - these will already be adjusted to maintain the shape
            float deltaX = mousePosition.x - initialMousePosForDrag.x;
            float deltaY = mousePosition.y - initialMousePosForDrag.y;
            
            // Apply the delta to the original position of each node
            float newX = currentNode3Data->originalPosition.x + deltaX;
            float newY = currentNode3Data->originalPosition.y + deltaY;
            
            // The boundary checks are done separately - in a first pass before this loop
            // We'll just use the adjusted deltas here
            currentNode3Data->position.x = newX;
            currentNode3Data->position.y = newY;
            
            // Update lines connected to this node
            struct list_item *line_head = NULL;
            for (line_head = line_list; line_head != NULL; line_head = line_head->next) {
              if (currentNode3Data->id == ((struct line *)line_head->data)->startNode.id) {
                ((struct line *)line_head->data)->startNode.position = currentNode3Data->position;
              }
              if (currentNode3Data->id == ((struct line *)line_head->data)->endNode.id) {
                ((struct line *)line_head->data)->endNode.position = currentNode3Data->position;
              }
            }
          }

          DrawCircleV(((struct node *)currentNode3->data)->position,
                      ((struct node *)currentNode3->data)->diameter,
                      ((struct node *)currentNode3->data)->color);
          DrawText(TextFormat("%d", ((struct node *)currentNode3->data)->id + 1),
                   ((struct node *)currentNode3->data)->position.x - 20,
                   ((struct node *)currentNode3->data)->position.y - 15, 8, BLACK);

          /* Multi-select draw selection circles*/
          if (isMultiSelect && currentNode3Data->selected) { 
            DrawCircleLines(currentNode3Data->position.x,
                            currentNode3Data->position.y,
                            currentNode3Data->diameter + (currentNode3Data->diameter / 2.0),
                            currentNode3Data->color);
          } else if (!isMultiSelect && currentNode3Data->selected) {
            currentNode3Data->selected = false;
          }
        }
      //Draw outline around selected node
      if (isNodeSelected && selectedNode != NULL)
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

      if (isSelecting)
        {
          DrawRectangleLinesEx(selectionRect, 2, BLUE); 
        }
      else
        {
          /* selectionRect.x = 0; */
          /* selectionRect.y = 0; */
          /* selectionRect.width = 0; */
          /* selectionRect.height = 0; */
        }
      /* if (line_list != NULL) */
      /*   DrawText(TextFormat("End node x pos: %f\nEnd node y pos: %f", ((struct line *)line_list->data)->endNode.position.x, ((struct line *)line_list->data)->endNode.position.y), 10, 10, 20, DARKGRAY); */

      /* DrawText(TextFormat("isLineStarted: %d", isLineStarted), 10, 50, 20, DARKGRAY); */

      EndDrawing();
      //----------------------------------------------------------------------------------
    }

  // De-Initialization
  //--------------------------------------------------------------------------------------
  cleanup_resources(&node_list, &line_list);
  CloseWindow(); // Close window and OpenGL context
  //--------------------------------------------------------------------------------------

  return 0;
}
