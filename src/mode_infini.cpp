#include "mode_infini.h"

#include "config.h"
#include "detection.h"
#include "leds.h"

// LED du milieu (index 2 sur 5)
const int LED_MILIEU = LED_PIN_TIR_COUNT / 2;

enum InfiniEtat { INFINI_EN_ATTENTE, INFINI_ANIMATION };

static InfiniEtat etat = INFINI_EN_ATTENTE;

// Chenillard symétrique : milieu → extérieur → milieu (×2)
static void animationChenillard() {
    for (int rep = 0; rep < 2; rep++) {
        // Expansion : milieu vers les extrémités
        digitalWrite(PIN_LED_TIR[LED_MILIEU], HIGH);
        delay(100);
        digitalWrite(PIN_LED_TIR[LED_MILIEU - 1], HIGH);
        digitalWrite(PIN_LED_TIR[LED_MILIEU + 1], HIGH);
        delay(100);
        digitalWrite(PIN_LED_TIR[LED_MILIEU - 2], HIGH);
        digitalWrite(PIN_LED_TIR[LED_MILIEU + 2], HIGH);
        delay(150);

        // Contraction : extrémités vers le milieu
        digitalWrite(PIN_LED_TIR[LED_MILIEU - 2], LOW);
        digitalWrite(PIN_LED_TIR[LED_MILIEU + 2], LOW);
        delay(100);
        digitalWrite(PIN_LED_TIR[LED_MILIEU - 1], LOW);
        digitalWrite(PIN_LED_TIR[LED_MILIEU + 1], LOW);
        delay(100);
        digitalWrite(PIN_LED_TIR[LED_MILIEU], LOW);
        delay(100);
    }
}

void infiniReset() {
    etat = INFINI_EN_ATTENTE;
    eteindre_all_leds_tir();
    setColor(0, 0, 0);
}

bool infiniEnAttente() {
    return etat == INFINI_EN_ATTENTE;
}

void modeInfini() {
    switch (etat) {
        case INFINI_EN_ATTENTE:
            if (cibleToucheeDebounce()) {
                etat = INFINI_ANIMATION;
            }
            break;

        case INFINI_ANIMATION:
            animationChenillard();
            etat = INFINI_EN_ATTENTE;
            break;
    }
}
