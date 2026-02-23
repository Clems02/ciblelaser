#include "mode_rapidfire.h"

#include "config.h"
#include "detection.h"
#include "leds.h"

// Délai avant allumage de la LED RGB (entre 1.5s et 6s)
const unsigned long RF_DELAI_MIN = 1500;
const unsigned long RF_DELAI_MAX = 6000;

// Durée d'allumage de la LED RGB (entre 2s et 4s)
const unsigned long RF_ALLUME_MIN = 1000;
const unsigned long RF_ALLUME_MAX = 3000;

// États du cycle rapidfire
enum RapidFireEtat {
    RF_ATTENTE_PREMIER_TIR,  // attente du tout premier tir (LED RGB éteinte)
    RF_DELAI,                // LED RGB éteinte, attente avant allumage
    RF_ALLUME                // LED RGB allumée, tir possible
};

static RapidFireEtat etat       = RF_ATTENTE_PREMIER_TIR;
static int           tirCount   = 0;
static unsigned long cycleStart = 0;
static unsigned long cycleDuree = 0;

// Tire un délai aléatoire entre min et max (hardware RNG de l'ESP32)
static unsigned long randomDuree(unsigned long minMs, unsigned long maxMs) {
    return minMs + (esp_random() % (maxMs - minMs + 1));
}

// Démarre la phase de délai (LED éteinte, attente aléatoire avant allumage)
static void demarrerDelai() {
    etat       = RF_DELAI;
    cycleStart = millis();
    cycleDuree = randomDuree(RF_DELAI_MIN, RF_DELAI_MAX);
    setColor(0, 0, 0);
    LOG("[RAPIDFIRE] Prochaine cible dans ~");
    LOG(cycleDuree / 1000);
    LOGLN("s...");
}

// Démarre la phase allumée (LED verte, tir possible pendant une durée aléatoire)
static void demarrerAllume() {
    etat       = RF_ALLUME;
    cycleStart = millis();
    cycleDuree = randomDuree(RF_ALLUME_MIN, RF_ALLUME_MAX);
    setColor(0, 255, 0);
    LOGLN("[RAPIDFIRE] TIREZ !");
}

void rapidfireReset() {
    etat       = RF_ATTENTE_PREMIER_TIR;
    tirCount   = 0;
    cycleStart = 0;
    cycleDuree = 0;
    eteindre_all_leds_tir();
    setColor(0, 0, 0);
    LOGLN("[RAPIDFIRE] En attente du premier tir...");
}

// Animation victoire
static void animationVictoire() {
    LOGLN("[RAPIDFIRE] VICTOIRE !");
    setColor(0, 0, 0);
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
    setColor(0, 255, 0);
    delay(2000);
    rapidfireReset();
}

void modeRapidfire() {
    switch (etat) {
        case RF_ATTENTE_PREMIER_TIR:
            // Premier tir : LED RGB éteinte, pas de contrainte de temps
            if (cibleToucheeDebounce()) {
                tirCount = 1;
                digitalWrite(PIN_LED_TIR[0], HIGH);
                LOG("[RAPIDFIRE] Premier tir ! 1/");
                LOGLN(LED_PIN_TIR_COUNT);

                if (tirCount >= LED_PIN_TIR_COUNT) {
                    animationVictoire();
                    return;
                }
                demarrerDelai();
            }
            break;

        case RF_DELAI:
            // Attente aléatoire avant allumage — tirs ignorés
            if (millis() - cycleStart >= cycleDuree) {
                demarrerAllume();
            }
            break;

        case RF_ALLUME:
            // LED RGB allumée — le joueur peut tirer
            if (millis() - cycleStart >= cycleDuree) {
                // Temps écoulé, trop tard
                setColor(0, 0, 0);
                LOGLN("[RAPIDFIRE] Trop lent ! On recommence le cycle...");
                demarrerDelai();
            } else if (cibleToucheeDebounce()) {
                // Tir réussi
                digitalWrite(PIN_LED_TIR[tirCount], HIGH);
                tirCount++;
                LOG("[RAPIDFIRE] Tir réussi ! ");
                LOG(tirCount);
                LOG("/");
                LOGLN(LED_PIN_TIR_COUNT);

                if (tirCount >= LED_PIN_TIR_COUNT) {
                    animationVictoire();
                    return;
                }
                demarrerDelai();
            }
            break;
    }
}
