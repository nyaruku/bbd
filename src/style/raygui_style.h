#pragma once

static void applyGenesisTheme() {
    // DEFAULT
    GuiSetStyle(DEFAULT, BORDER_COLOR_NORMAL,   0x667384ff);
    GuiSetStyle(DEFAULT, BASE_COLOR_NORMAL,     0x181b1eff);
    GuiSetStyle(DEFAULT, TEXT_COLOR_NORMAL,     0xc2c8d0ff);
    GuiSetStyle(DEFAULT, BORDER_COLOR_FOCUSED,  0xd3dbdfff);
    GuiSetStyle(DEFAULT, BASE_COLOR_FOCUSED,    0xa7afb0ff);
    GuiSetStyle(DEFAULT, TEXT_COLOR_FOCUSED,    0x020202ff);
    GuiSetStyle(DEFAULT, BORDER_COLOR_PRESSED,  0x181b1eff);
    GuiSetStyle(DEFAULT, BASE_COLOR_PRESSED,    0xac3c3cff);
    GuiSetStyle(DEFAULT, TEXT_COLOR_PRESSED,    0xdededeff);
    GuiSetStyle(DEFAULT, BORDER_COLOR_DISABLED, 0x3e4550ff);
    GuiSetStyle(DEFAULT, BASE_COLOR_DISABLED,   0x2e353dff);
    GuiSetStyle(DEFAULT, TEXT_COLOR_DISABLED,   0x484f57ff);
    GuiSetStyle(DEFAULT, TEXT_SIZE,             16);
    GuiSetStyle(DEFAULT, TEXT_SPACING,          0);
    GuiSetStyle(DEFAULT, LINE_COLOR,            0x96a3b4ff);
    GuiSetStyle(DEFAULT, BACKGROUND_COLOR,      0x292c33ff);
    GuiSetStyle(DEFAULT, TEXT_LINE_SPACING,     8);
    // LABEL
    GuiSetStyle(LABEL,   TEXT_COLOR_FOCUSED,    0x97a9aeff);
    // SLIDER
    GuiSetStyle(SLIDER,  TEXT_COLOR_FOCUSED,    0xa69a9aff);
    GuiSetStyle(SLIDER,  BORDER_COLOR_PRESSED,  0xc3ccd5ff);
    // CHECKBOX
    GuiSetStyle(CHECKBOX, TEXT_COLOR_FOCUSED,   0xa7afb0ff);
    GuiSetStyle(CHECKBOX, BORDER_COLOR_PRESSED, 0xa7aeb5ff);
    // TEXTBOX
    GuiSetStyle(TEXTBOX,  TEXT_COLOR_FOCUSED,   0xa9a5a5ff);
    // VALUEBOX
    GuiSetStyle(VALUEBOX, TEXT_COLOR_FOCUSED,   0xc9c7c7ff);
}
