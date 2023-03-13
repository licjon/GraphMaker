#include "raylib.h"

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 450;
    const int maxBalls = 10;
    InitWindow(screenWidth, screenHeight, "raylib [core] example - mouse input");

    struct ball {
      Vector2 position;
      int diameter;
      Color color;
    };

    struct ball balls[maxBalls];
    int ballIndex = 0;
    int ballDiameter = 10;
    Vector2 mousePosition = { -100.0f, -100.0f };
    Color ballColor = DARKBLUE;
    bool isBallDrawn = false;
    bool isNodeSelected = false;
    bool isNodeLocked = false;

    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    //---------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        mousePosition = GetMousePosition();

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) isBallDrawn = true;
        else if (IsMouseButtonPressed(MOUSE_BUTTON_MIDDLE)) ballColor = LIME;
        else if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
          if (isNodeSelected) {
            isNodeSelected = false;
            isNodeLocked = false;
          }
          else
            isNodeSelected = true;
        }
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
        
            ClearBackground(RAYWHITE);

            if (isBallDrawn) {
              balls[ballIndex].color = ballColor;
              balls[ballIndex].diameter = ballDiameter;
              balls[ballIndex].position = mousePosition;
              ballIndex++;
              isBallDrawn = false;
            }

            int selectedIndex;
            for (int i = 0; i < maxBalls; i++) {
                for (int j = 0; j < maxBalls; j++) {
                  if (!CheckCollisionCircles(balls[i].position, ballDiameter/2.0, balls[j].position, ballDiameter/2.0)) {
                    if (CheckCollisionPointCircle(mousePosition, balls[i].position, ballDiameter/2.0) && isNodeSelected && !isNodeLocked) { 
                      selectedIndex = i;
                      isNodeLocked = true;
                      break;
                    }
                  }
                  else {
                    DrawCircleV(balls[i].position, balls[i].diameter, balls[i].color);
                  }
                }
            }

            if (isNodeSelected) {
              balls[selectedIndex].position = mousePosition;
              DrawCircleV(balls[selectedIndex].position, balls[selectedIndex].diameter, balls[selectedIndex].color);
            }

            DrawText("Graph Maker", 10, 10, 20, DARKGRAY);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}
