#include "mode_decontracte.h"

#include "config.h"
#include "detection.h"
#include "leds.h"

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
    delay(500);

    setColor(0, 0, 0);

    for (int x = 0; x < 3; x++) {
        for (int i = LED_PIN_TIR_COUNT - 1; i >= 0; i--) {
            digitalWrite(PIN_LED_TIR[i], LOW);
            delay(75);
        }
        for (int j = 0; j < LED_PIN_TIR_COUNT; j++) {
            digitalWrite(PIN_LED_TIR[j], HIGH);
            delay(75);
        }
    }

    delay(300);

    for (int x = 0; x < 3; x++) {
        for (int i = LED_PIN_TIR_COUNT - 1; i >= 0; i--) {
            digitalWrite(PIN_LED_TIR[i], LOW);
        }
        delay(100);
        for (int j = 0; j < LED_PIN_TIR_COUNT; j++) {
            digitalWrite(PIN_LED_TIR[j], HIGH);
        }
        delay(100);
    }

    delay(2000);

    decontracteReset();
}

bool decontracteEnAttente() {
    return !gameRunning;
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
