#include <cstdlib>
#include <raylib/raylib.h>
#define RAYGUI_IMPLEMENTATION
#include <raylib/raygui.h>
#include <json.hpp>

int main() {

    InitWindow(800, 450, "raylib example - basic window");
    GuiLoadStyle("resources/style.rgs");
    SetWindowState(FLAG_WINDOW_RESIZABLE);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);
        EndDrawing();
    }

    CloseWindow();

    return 0;
}