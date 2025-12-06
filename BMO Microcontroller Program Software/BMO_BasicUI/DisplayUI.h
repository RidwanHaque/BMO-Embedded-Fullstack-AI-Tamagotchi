#ifndef DISPLAY_UI_H
#define DISPLAY_UI_H

#include "Config.h"

void drawScreen();
void updateMenuCursor();
void updateBarFill(int x, int y, int w, int val, word color);
void updateTime();
void drawFace();

// Chat window API
void showBMOReplyOnScreen(const char* text);
void scrollChatUp();
void scrollChatDown();
void clearChat();
void drawBackOverlay();


#endif
