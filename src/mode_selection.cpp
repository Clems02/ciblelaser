#include "mode_selection.h"
#include "config.h"
#include "leds.h"
#include "mode_classique.h"
#include "mode_decontracte.h"
#include "mode_rapidfire.h"
#include "mode_survie.h"

int currentMode = 0;

// Tableau de pointeurs de fonctions indexé par le mode
static void (*modeFunctions[NB_MODES])() = {modeClassique, modeDecontracte, modeRapidfire, modeSurvie};

// Sélection de mode (fenêtre de 3s avec clignotement)
static bool          enSelectionMode    = false;
static unsigned long selectionModeStart = 0;
static unsigned long lastBlinkTime      = 0;
static bool          blinkState         = false;
const unsigned long  SELECTION_DURATION = 3000;
const unsigned long  BLINK_INTERVAL     = 200;

// Debounce bouton
static unsigned long       lastButtonPress = 0;
static const unsigned long DEBOUNCE_DELAY  = 50;
static int                 lastButtonState = HIGH;

// Reset tous les modes de jeu
static void resetTousLesModes() {
    classiqueReset();
    decontracteReset();
    rapidfireReset();
    survieReset();
}

bool gererSelectionMode() {
    int buttonState = digitalRead(PIN_BOUTON);

    if (buttonState == LOW && lastButtonState == HIGH && (millis() - lastButtonPress) > DEBOUNCE_DELAY) {
        lastButtonPress = millis();
        currentMode     = (currentMode + 1) % NB_MODES;

        // Reset du jeu en cours et entrer en mode sélection
        resetTousLesModes();
        enSelectionMode    = true;
        selectionModeStart = millis();
        blinkState         = false;
        lastBlinkTime      = millis();
    }
    lastButtonState = buttonState;

    // --- Mode sélection : clignotement des LEDs TIR pendant 3s ---
    if (enSelectionMode) {
        if (millis() - selectionModeStart >= SELECTION_DURATION) {
            enSelectionMode = false;
            eteindre_all_leds_tir();
            setColor(0, 0, 0);
            resetTousLesModes();
        } else if (millis() - lastBlinkTime >= BLINK_INTERVAL) {
            lastBlinkTime = millis();
            blinkState    = !blinkState;

            for (int i = 0; i < LED_PIN_TIR_COUNT; i++) {
                digitalWrite(PIN_LED_TIR[i], (i <= currentMode && blinkState) ? HIGH : LOW);
            }
        }
        return true;
    }

    return false;
}

void executerModeCourant() {
    modeFunctions[currentMode]();
}
