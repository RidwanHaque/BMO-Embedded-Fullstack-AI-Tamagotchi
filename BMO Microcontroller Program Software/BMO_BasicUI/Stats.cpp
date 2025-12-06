#include "Stats.h"
#include "Config.h"
#include "DisplayUI.h"
#include "LED.h"

// extern globals
extern int energy;
extern int happiness;

unsigned long lastDecay = 0;
const unsigned long DECAY_INTERVAL = 5000;   // 5 seconds

int clamp(int v) {
    if (v < 0) return 0;
    if (v > 100) return 100;
    return v;
}

void initStats() {
    lastDecay = millis();
}

void updateStats() {
    unsigned long now = millis();

    if (now - lastDecay >= DECAY_INTERVAL) {
        happiness = clamp(happiness - 1);
        energy    = clamp(energy - 1);

        // Update LED mood when stats change
        ledSetMood(happiness);

        // Only update bar fills + face, not whole screen or menu
        updateBarFill(58,  2, 65, energy,   BLUE);    // Energy bar
        updateBarFill(58, 10, 65, happiness, YELLOW); // Happy bar
        drawFace();                                   // face depends on happiness

        lastDecay = now;
    }
}
