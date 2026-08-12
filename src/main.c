#include "raylib.h"

int main(void)
{
    InitWindow(800, 450, "raylib example - basic window");
    SetWindowState(FLAG_WINDOW_RESIZABLE);

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(BLACK);
        DrawText("Batch Beatmap Downloader", 190, 200, 20, LIGHTGRAY);
        DrawText(TextFormat("Frame Time: %.1f", GetFrameTime()*1000), 190, 230, 20, LIGHTGRAY);
        DrawText(TextFormat("FPS: %i", GetFPS()), 190, 260, 20, LIGHTGRAY);
        EndDrawing();
    }

    CloseWindow();

    return 0;
}