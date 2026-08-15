#pragma once

// Just to draw text at a more intuitive way
static inline void DrawText(const Font &font, const std::string &text, const int posX, const int posY, const int fontSize, const Color tint) {
    DrawTextEx(font, text.c_str(), Vector2(posX, posY), fontSize, 0, tint);
}