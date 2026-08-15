#include <cstdlib>
#include "raylib.h"

int main()
{
    // don't remove
    setenv("SDL_JOYSTICK_HIDAPI", "0", 1);

    InitWindow(800, 450, "raylib example - basic window");
    SetWindowState(FLAG_WINDOW_RESIZABLE);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);
        EndDrawing();
    }

    CloseWindow();

    return 0;
}