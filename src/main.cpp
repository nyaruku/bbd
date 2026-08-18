#include <cstdlib>
#include <raylib/raylib.h>
#define RAYGUI_IMPLEMENTATION
#include <raylib/raygui.h>
#include <json.hpp>
#include "fonts/jet_brains_mono_regular.h"
#include "style/raygui_style.h"
#include "helpers/raylib.h"

int main() {

    InitWindow(800, 450, "batch beatmap downloader");

    const Font font = LoadFontFromMemory(".ttf", JET_BRAINS_MONO_REGULAR_DATA, JET_BRAINS_MONO_REGULAR_DATA_LEN, 30, nullptr, 0);
    GuiSetFont(font);
    applyGenesisTheme();

    SetWindowState(FLAG_WINDOW_RESIZABLE);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));
        DrawText(font, "Hello, Genesis Theme!", 10, 10, 30, GetColor(GuiGetStyle(DEFAULT, TEXT_COLOR_NORMAL)));
        EndDrawing();
    }

    UnloadFont(font);
    CloseWindow();

    return 0;
}