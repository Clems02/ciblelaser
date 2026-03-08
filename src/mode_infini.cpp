#include "mode_infini.h"

#include "config.h"
#include "detection.h"
#include "leds.h"

// Durée totale de l'animation de validation (2s)
const unsigned long INFINI_BLINK_DURATION = 2000;
// LED allumée 300ms, éteinte 100ms → clignotement léger
const unsigned long INFINI_BLINK_ON  = 300;
const unsigned long INFINI_BLINK_OFF = 100;

// LED du milieu (index 2 sur 5)
const int LED_MILIEU = LED_PIN_TIR_COUNT / 2;

enum InfiniEtat { INFINI_EN_ATTENTE, INFINI_CLIGNOTEMENT };

static InfiniEtat    etat       = INFINI_EN_ATTENTE;
static unsigned long blinkStart = 0;
static unsigned long lastToggle = 0;
static bool          ledOn      = false;

void infiniReset() {
    etat       = INFINI_EN_ATTENTE;
    blinkStart = 0;
    lastToggle = 0;
    ledOn      = false;
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
                etat       = INFINI_CLIGNOTEMENT;
                blinkStart = millis();
                lastToggle = millis();
                ledOn      = true;
                digitalWrite(PIN_LED_TIR[LED_MILIEU], HIGH);
            }
            break;

        case INFINI_CLIGNOTEMENT:
            if (millis() - blinkStart >= INFINI_BLINK_DURATION) {
                // Animation terminée : éteindre et repasser en attente → light sleep reprend
                digitalWrite(PIN_LED_TIR[LED_MILIEU], LOW);
                etat = INFINI_EN_ATTENTE;
            } else {
                // Clignotement non-bloquant
                unsigned long toggleInterval = ledOn ? INFINI_BLINK_ON : INFINI_BLINK_OFF;
                if (millis() - lastToggle >= toggleInterval) {
                    ledOn = !ledOn;
                    digitalWrite(PIN_LED_TIR[LED_MILIEU], ledOn ? HIGH : LOW);
                    lastToggle = millis();
                }
            }
            break;
    }
}
