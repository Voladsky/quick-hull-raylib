/*******************************************************************************************
*
*   raylib [core] example - Basic window
*
*   Welcome to raylib!
*
*   To test examples, just press F6 and execute raylib_compile_execute script
*   Note that compiled executable is placed in the same folder as .c file
*
*   You can find all basic examples on C:\raylib\raylib\examples folder or
*   raylib official webpage: www.raylib.com
*
*   Enjoy using raylib. :)
*
*   Example originally created with raylib 1.0, last time updated with raylib 1.0
*
*   Example licensed under an unmodified zlib/libpng license, which is an OSI-certified,
*   BSD-like license that allows static linking with closed source software
*
*   Copyright (c) 2013-2023 Ramon Santamaria (@raysan5)
*
********************************************************************************************/

#include "raylib.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define FPS 30

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void) {
    srand(time(0));
    // Initialization
    //-------------------------------------------------------------------------------------
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "raylib [core] example - basic window");
    SetTargetFPS(FPS);
    
    //--------------------------------------------------------------------------------------

    double* x;
    double* y;
    int* pt;
    int sz = 0;
    
    // Main app loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        
        // Update
        //----------------------------------------------------------------------------------
         if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            Vector2 point = GetMousePosition();
            sz++;
            char s[10];
            sprintf(s, "%d", sz);
            TraceLog(LOG_INFO, s);
            if (sz == 1) {
                pt = (int*)malloc(sizeof(int));
                x = (double*)malloc(sizeof(double));
                y = (double*)malloc(sizeof(double));
            }
            else {
                pt = (int*)realloc(pt, sz * sizeof(int));
                x = (double*)realloc(x, sz * sizeof(double));
                y = (double*)realloc(y, sz * sizeof(double));
            }
            pt[sz - 1] = sz - 1;
            x[sz - 1] = point.x;
            y[sz - 1] = point.y;
            
            
         }
        
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
        

             ClearBackground(RAYWHITE);
             DrawText("HIIIII", 0, 0, 12, BLACK);
             for (int i = 0; i < sz; ++i) {
                DrawCircle(x[i], y[i], 5, RED);
             }
        EndDrawing();
        
        //----------------------------------------------------------------------------------
    }
    free(pt);
    free(x);
    free(y);
    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------
    return 0;
}