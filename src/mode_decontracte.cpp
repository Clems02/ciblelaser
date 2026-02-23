#include "mode_decontracte.h"
#include "config.h"
#include "leds.h"
#include "detection.h"

// État du mode décontracté
static bool gameRunning = false;
static int  tirCount    = 0;

void decontracteReset() {
    gameRunning = false;
    tirCount    = 0;
    eteindre_all_leds_tir();
    setColor(0, 0, 0);
    LOGLN("[DECONTRACTE] En attente du premier tir...");
}

// Animation victoire : chenillard rapide 3 fois
static void animationVictoire() {
    LOGLN("[DECONTRACTE] VICTOIRE !");
    for (int r = 0; r < 3; r++) {
        for (int i = 0; i < LED_PIN_TIR_COUNT; i++) {
            digitalWrite(PIN_LED_TIR[i], HIGH);
            delay(60);
            digitalWrite(PIN_LED_TIR[i], LOW);
        }
    }
    for (int i = 0; i < LED_PIN_TIR_COUNT; i++) {
        digitalWrite(PIN_LED_TIR[i], HIGH);
    }
    delay(2000);
    decontracteReset();
}

void modeDecontracte() {
    // Phase d'attente du premier tir
    if (!gameRunning) {
        if (cibleToucheeDebounce()) {
            gameRunning = true;
            tirCount    = 1;
            digitalWrite(PIN_LED_TIR[0], HIGH);
            LOG("[DECONTRACTE] Tir réussi ! 1/");
            LOGLN(LED_PIN_TIR_COUNT);
        }
        return;
    }

    // Détection de tir avec anti-rebond
    if (cibleToucheeDebounce()) {
        digitalWrite(PIN_LED_TIR[tirCount], HIGH);
        tirCount++;
        LOG("[DECONTRACTE] Tir réussi ! ");
        LOG(tirCount);
        LOG("/");
        LOGLN(LED_PIN_TIR_COUNT);

        if (tirCount >= LED_PIN_TIR_COUNT) {
            animationVictoire();
            return;
        }
    }
}
