#include "Sound.h"
#include "Config.h"

#include <driver/ledc.h>

static const ledc_timer_t    SOUND_TIMER   = LEDC_TIMER_0;
static const ledc_channel_t  SOUND_CHANNEL = LEDC_CHANNEL_0;

// --- Initialize Speaker PWM ---
void initSound() {

    pinMode(SPEAKER_PIN, OUTPUT);  // 

    ledc_timer_config_t timer_cfg = {
        .speed_mode       = LEDC_LOW_SPEED_MODE,
        .duty_resolution  = LEDC_TIMER_10_BIT,
        .timer_num        = SOUND_TIMER,
        .freq_hz          = 2000,       // default 2 kHz
        .clk_cfg          = LEDC_AUTO_CLK
    };
    ledc_timer_config(&timer_cfg);

    ledc_channel_config_t channel_cfg = {
        .gpio_num       = SPEAKER_PIN,
        .speed_mode     = LEDC_LOW_SPEED_MODE,
        .channel        = SOUND_CHANNEL,
        .intr_type      = LEDC_INTR_DISABLE,
        .timer_sel      = SOUND_TIMER,
        .duty           = 0,
        .hpoint         = 0
    };
    ledc_channel_config(&channel_cfg);
}

// --- Internal tone helper ---
static void playFreq(uint32_t freq, uint32_t ms) {

    ledc_set_freq(LEDC_LOW_SPEED_MODE, SOUND_TIMER, freq);
    ledc_set_duty(LEDC_LOW_SPEED_MODE, SOUND_CHANNEL, 512); // half volume
    ledc_update_duty(LEDC_LOW_SPEED_MODE, SOUND_CHANNEL);

    delay(ms);

    // turn off
    ledc_set_duty(LEDC_LOW_SPEED_MODE, SOUND_CHANNEL, 0);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, SOUND_CHANNEL);
}

// === PUBLIC FUNCTIONS ===

void soundMenuMove() {
    playFreq(1800, 30);
}

void soundMenuSelect() {
    playFreq(2200, 60);
}

void soundFood() {
    playFreq(900, 40);
    playFreq(1400, 40);
}

void soundGameOver() {
    playFreq(600, 150);
    playFreq(300, 200);
}


void soundVoiceReply() {
    // Little ascending triad to indicate a chatbot reply
    playFreq(880, 80);
    playFreq(1200, 80);
    playFreq(1500, 120);
}


void soundPlayAdventureTheme() {
    // F#4, G4, A4, D5 (one octave above D4), B4

    playFreq(370, 200);  // F#4 ≈ 369.99 Hz
    playFreq(392, 200);  // G4  ≈ 391.99 Hz
    playFreq(440, 700);  // A4  = 440 Hz
    playFreq(587, 450);  // D5  ≈ 587.33 Hz (next octave)
    playFreq(494, 700);  // B4  ≈ 493.88 Hz
        // Added part: D5 E5 F#5 E5 D5
    playFreq(587, 200);  // D5 ≈ 587.33 Hz
    playFreq(659, 200);  // E5 ≈ 659.26 Hz
    playFreq(740, 400);  // F#5 ≈ 739.99 Hz
    playFreq(659, 200);  // E5
    playFreq(587, 300);  // D5 (resolve)
    playFreq(659, 300);  // E5
    delay(50); 
    playFreq(659, 300);  // E5
    playFreq(587, 700);  // D5 (resolve)
}

