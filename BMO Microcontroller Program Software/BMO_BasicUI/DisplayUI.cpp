#include "DisplayUI.h"
#include "Config.h"
#include "TimeSystem.h"

// Section boundaries
const int TOP_BAR_BOTTOM = 20;
const int FACE_TOP       = 25;
const int FACE_BOTTOM    = 100;
const int MENU_TOP       = 95;

// Face constants
const int CX    = 64;
const int EYE_Y = 55;

// Chat window state
static String chatText = "";
static int chatScroll = 0;
static const int CHAT_MAX_LINES = 6;   // rows you can show

// -------------------------------------
// Internal: draw chat window region
// -------------------------------------
static void drawChatWindow() {
    if (chatText.length() == 0) {
        return; // no chat yet, don't paint over menu
    }

    const int topY    = MENU_TOP;
    const int bottomY = 127;
    Display.gfx_RectangleFilled(0, topY, 127, bottomY, BLACK);

    // naive wrap into fixed-length lines
    const int maxCols = 10;
    String lines[16];
    int lineCount = 0;
    String current = "";

    for (size_t i = 0; i < chatText.length(); i++) {
        char c = chatText[i];
        if (c == '\n') {
            if (lineCount < 16) lines[lineCount++] = current;
            current = "";
        } else {
            current += c;
            if ((int)current.length() >= maxCols && c == ' ') {
                if (lineCount < 16) lines[lineCount++] = current;
                current = "";
            }
        }
        if (lineCount >= 16) break;
    }
    if (current.length() && lineCount < 16) {
        lines[lineCount++] = current;
    }

    if (lineCount == 0) return;

    if (chatScroll < 0) chatScroll = 0;
    if (chatScroll > lineCount - 1) chatScroll = lineCount - 1;

    int firstLine = chatScroll;
    int lastLine  = min(firstLine + CHAT_MAX_LINES, lineCount);

    int row = 13; // rows just above menu (13+)
    Display.txt_FGcolour(WHITE);

    for (int i = firstLine; i < lastLine; i++) {
        Display.txt_MoveCursor(row, 0);
        Display.print("                ");  // clear row
        Display.txt_MoveCursor(row, 0);
        Display.print(lines[i]);
        row++;
    }
}

// -------------------------------------
// Curved smile and frown helpers
// -------------------------------------
void drawSmoothSmile(int y) {
    Display.gfx_Line(CX - 12, y,     CX - 6,  y + 3, BMO_MOUTH_COLOR);
    Display.gfx_Line(CX - 6,  y + 3, CX,      y + 4, BMO_MOUTH_COLOR);
    Display.gfx_Line(CX,      y + 4, CX + 6,  y + 3, BMO_MOUTH_COLOR);
    Display.gfx_Line(CX + 6,  y + 3, CX + 12, y,     BMO_MOUTH_COLOR);
}

void drawSmoothFrown(int y) {
    // Left downward curve
    Display.gfx_Line(CX - 12, y - 2, CX - 8,  y - 5, BMO_MOUTH_COLOR);
    Display.gfx_Line(CX - 8,  y - 5, CX - 4,  y - 7, BMO_MOUTH_COLOR);

    // Deep middle
    Display.gfx_Line(CX - 4,  y - 7, CX + 4,  y - 7, BMO_MOUTH_COLOR);

    // Right upward curve
    Display.gfx_Line(CX + 4,  y - 7, CX + 8,  y - 5, BMO_MOUTH_COLOR);
    Display.gfx_Line(CX + 8,  y - 5, CX + 12, y - 2, BMO_MOUTH_COLOR);
}

// Big U-shaped open grin (outlined + filled black)
void drawOpenGrin(int y) {
    // 1) Outline in BMO_MOUTH_COLOR

    // Top horizontal
    Display.gfx_Line(CX - 14, y - 6, CX + 14, y - 6, BMO_MOUTH_COLOR);

    // Left side curve
    Display.gfx_Line(CX - 14, y - 6, CX - 16, y - 2, BMO_MOUTH_COLOR);
    Display.gfx_Line(CX - 16, y - 2, CX - 16, y + 4, BMO_MOUTH_COLOR);
    Display.gfx_Line(CX - 16, y + 4, CX - 14, y + 8, BMO_MOUTH_COLOR);

    // Bottom curve
    Display.gfx_Line(CX - 14, y + 8, CX - 8,  y + 10, BMO_MOUTH_COLOR);
    Display.gfx_Line(CX - 8,  y + 10, CX,      y + 11, BMO_MOUTH_COLOR);
    Display.gfx_Line(CX,      y + 11, CX + 8,  y + 10, BMO_MOUTH_COLOR);
    Display.gfx_Line(CX + 8,  y + 10, CX + 14, y + 8,  BMO_MOUTH_COLOR);

    // Right side curve
    Display.gfx_Line(CX + 14, y + 8, CX + 16, y + 4, BMO_MOUTH_COLOR);
    Display.gfx_Line(CX + 16, y + 4, CX + 16, y - 2, BMO_MOUTH_COLOR);
    Display.gfx_Line(CX + 16, y - 2, CX + 14, y - 6, BMO_MOUTH_COLOR);

    // 2) Fill interior with black:
    //    a) A smaller inner U in BLACK to close gaps
    Display.gfx_Line(CX - 13, y - 5, CX + 13, y - 5, BLACK);
    Display.gfx_Line(CX - 13, y - 5, CX - 15, y - 1, BLACK);
    Display.gfx_Line(CX - 15, y - 1, CX - 15, y + 3, BLACK);
    Display.gfx_Line(CX - 15, y + 3, CX - 13, y + 7, BLACK);
    Display.gfx_Line(CX - 13, y + 7, CX - 7,  y + 9, BLACK);
    Display.gfx_Line(CX - 7,  y + 9, CX + 7,  y + 9, BLACK);
    Display.gfx_Line(CX + 7,  y + 9, CX + 13, y + 7, BLACK);
    Display.gfx_Line(CX + 13, y + 7, CX + 15, y + 3, BLACK);
    Display.gfx_Line(CX + 15, y + 3, CX + 15, y - 1, BLACK);
    Display.gfx_Line(CX + 15, y - 1, CX + 13, y - 5, BLACK);

    //    b) A filled rectangle inside that inner U
    int left   = CX - 13;
    int right  = CX + 13;
    int top    = y - 4;
    int bottom = y + 8;
    Display.gfx_RectangleFilled(left, top, right, bottom, BLACK);
}

// -------------------------------------
// BMO Eyes (always black dots)
// -------------------------------------
void drawEyes(int rWhite, int rBlack) { //arent using white anymore
    int r = rBlack;
    Display.gfx_CircleFilled(CX - 14, EYE_Y, r, BMO_EYE_COLOR);
    Display.gfx_CircleFilled(CX + 14, EYE_Y, r, BMO_EYE_COLOR);
}

// -------------------------------------
// Draw face
// -------------------------------------
void drawFace() {
    extern int happiness;
    extern int energy;

    Display.gfx_RectangleFilled(0, FACE_TOP, 127, FACE_BOTTOM, BMO_SCREEN_COLOR);

    drawEyes(0, 3);

    int mood = (energy < happiness) ? energy : happiness;

    if (mood >= 80) {
        drawOpenGrin(75);
    } else if (mood >= 50) {
        drawSmoothSmile(75);
    } else if (mood >= 20) {
        Display.gfx_Line(CX - 10, 75, CX + 10, 75, BMO_MOUTH_COLOR);
    } else {
        drawSmoothFrown(78);
    }
}

// -------------------------------------
// Draw bar (frame + fill)
// -------------------------------------
void drawBar(int x, int y, int w, int val, word color) {
    int h = 6;
    Display.gfx_Rectangle(x, y, x + w, y + h, WHITE);
    int fill = (w * val) / 100;
    if (fill > 0)
        Display.gfx_RectangleFilled(x + 1, y + 1, x + fill - 1, y + h - 1, color);
}

// -------------------------------------
// Full UI Draw
// -------------------------------------
void drawScreen() {
    extern AppState appState;
    extern int energy;
    extern int happiness;
    extern int menuIndex;
    extern const int MENU_ITEMS;
    extern const char* menuOptions[];
    extern const int SETTINGS_ITEMS;
    extern const char* settingsOptions[];
    extern const int GAMES_ITEMS;
    extern const char* gamesOptions[];

    // Top bar
    Display.gfx_RectangleFilled(0, 0, 127, TOP_BAR_BOTTOM, BLACK);

    Display.txt_FGcolour(WHITE);
    Display.txt_MoveCursor(0, 0);
    Display.print("Energy:");
    drawBar(58, 2, 65, energy, BLUE);

    Display.txt_MoveCursor(2, 0);
    Display.print(getTimeString());

    Display.txt_MoveCursor(1, 0);
    Display.print("Happy :");
    drawBar(58, 10, 65, happiness, YELLOW);

    // Face
    drawFace();

    // Menu region
    Display.gfx_RectangleFilled(0, MENU_TOP, 127, 127, BLACK);

    // Decide which menu to draw
    const char** currentMenu = nullptr;
    int count = 0;

    if (appState == STATE_MAIN_MENU) {
        currentMenu = menuOptions;
        count = MENU_ITEMS;
    } else if (appState == STATE_SETTINGS_MENU) {
        currentMenu = settingsOptions;
        count = SETTINGS_ITEMS;
    } else if (appState == STATE_GAMES_MENU) {
        currentMenu = gamesOptions;
        count = GAMES_ITEMS;
    } else {
        return; // A game is running (no menu)
    }

    // Draw menu with scrolling window
    const int VISIBLE_ROWS = 3;
    int firstItem = 0;

    if (menuIndex >= VISIBLE_ROWS) {
        firstItem = menuIndex - VISIBLE_ROWS + 1;
    }
    int lastItem = firstItem + VISIBLE_ROWS;
    if (lastItem > count) lastItem = count;

    // Clear visible menu rows
    for (int row = 0; row < VISIBLE_ROWS; row++) {
        Display.txt_MoveCursor(13 + row, 0);
        Display.txt_FGcolour(WHITE);
        Display.print("                  ");
    }

    // Draw only visible subset
    for (int row = 0, i = firstItem; i < lastItem; i++, row++) {
        Display.txt_MoveCursor(13 + row, 0);

        if (i == menuIndex) {
            Display.txt_FGcolour(TURQUOISE);
            Display.print("> ");
        } else {
            Display.txt_FGcolour(WHITE);
            Display.print("  ");
        }

        Display.print(currentMenu[i]);
    }

    // Redraw chat window (if any)
    drawChatWindow();
}

// -------------------------------------
// Update only menu cursor/text region
// -------------------------------------
void updateMenuCursor() {
    extern AppState appState;
    extern int menuIndex;
    extern const int MENU_ITEMS;
    extern const char* menuOptions[];
    extern const int SETTINGS_ITEMS;
    extern const char* settingsOptions[];
    extern const int GAMES_ITEMS;
    extern const char* gamesOptions[];

    const char** currentMenu = nullptr;
    int count = 0;

    if (appState == STATE_MAIN_MENU) {
        currentMenu = menuOptions;
        count = MENU_ITEMS;
    } else if (appState == STATE_SETTINGS_MENU) {
        currentMenu = settingsOptions;
        count = SETTINGS_ITEMS;
    } else if (appState == STATE_GAMES_MENU) {
        currentMenu = gamesOptions;
        count = GAMES_ITEMS;
    } else {
        return; // no menu to draw
    }

    const int VISIBLE_ROWS = 3;
    int firstItem = 0;

    if (menuIndex >= VISIBLE_ROWS) {
        firstItem = menuIndex - VISIBLE_ROWS + 1;
    }
    int lastItem = firstItem + VISIBLE_ROWS;
    if (lastItem > count) lastItem = count;

    // Clear visible menu rows
    for (int row = 0; row < VISIBLE_ROWS; row++) {
        Display.txt_MoveCursor(13 + row, 0);
        Display.txt_FGcolour(WHITE);
        Display.print("                  ");
    }

    // Redraw subset with correct cursor
    for (int row = 0, i = firstItem; i < lastItem; i++, row++) {
        Display.txt_MoveCursor(13 + row, 0);

        if (i == menuIndex) {
            Display.txt_FGcolour(TURQUOISE);
            Display.print("> ");
        } else {
            Display.txt_FGcolour(WHITE);
            Display.print("  ");
        }

        Display.print(currentMenu[i]);
    }
}

// -------------------------------------
// Update only bar interior (keep label & frame)
// -------------------------------------
void updateBarFill(int x, int y, int w, int val, word color) {
    int h = 6;
    int fill = (w * val) / 100;

    Display.gfx_RectangleFilled(x + 1, y + 1, x + w - 1, y + h - 1, BLACK);

    if (fill > 0) {
        Display.gfx_RectangleFilled(x + 1, y + 1,
                                    x + fill - 1, y + h - 1, color);
    }
}

// -------------------------------------
// Update only time string on top bar
// -------------------------------------
void updateTime() {
    Display.txt_FGcolour(WHITE);
    Display.txt_MoveCursor(2, 0);
    Display.print("      ");
    Display.txt_MoveCursor(2, 0);
    Display.print(getTimeString());
}

// -------------------------------------
// Chat API
// -------------------------------------
void showBMOReplyOnScreen(const char* text) {
    chatText   = text ? String(text) : String("");
    chatScroll = 0;
    drawChatWindow();
}

void scrollChatUp() {
    if (chatScroll > 0) {
        chatScroll--;
        drawChatWindow();
    }
}

void scrollChatDown() {
    chatScroll++;
    drawChatWindow();
}

void clearChat() {
    chatText = "";
    chatScroll = 0;

    // Clear menu area (chat region) back to black;
    // drawScreen() or updateMenuCursor() will redraw menu text.
    Display.gfx_RectangleFilled(0, MENU_TOP, 127, 127, BLACK);
}

// -------------------------------------
// Back overlay
// -------------------------------------
void drawBackOverlay() {
    // Clear menu area
    Display.gfx_RectangleFilled(0, MENU_TOP, 127, 127, BLACK);
    Display.txt_FGcolour(WHITE);
    Display.txt_MoveCursor(13, 0);
    Display.print("> Back (up)");
}
