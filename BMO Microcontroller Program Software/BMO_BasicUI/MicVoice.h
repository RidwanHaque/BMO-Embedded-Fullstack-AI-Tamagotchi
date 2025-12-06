#ifndef MIC_VOICE_H
#define MIC_VOICE_H

#include <Arduino.h>

void initMicVoice();     // call once in setup()
void micVoiceLoop();     // call each loop() when not in Snake

#endif
