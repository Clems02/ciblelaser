#include "mode_classique.h"
#include "config.h"
#include "leds.h"
#include "detection.h"

// Couleurs du timer : vert → jaune → orange → rouge, 10s chacune (40s total)
const unsigned long CLASSIQUE_PHASE_DURATION                   = 10000;
const int           CLASSIQUE_NB_PHASES                        = 4;
const uint8_t       CLASSIQUE_COULEURS[CLASSIQUE_NB_PHASES][3] = {
    {0, 255, 0},    // vert
    {255, 255, 0},  // jaune
    {255, 128, 0},  // orange
    {255, 0, 0}     // rouge
};

// État du mode classique
static bool          gameRunning = false;  // false = attente premier tir, true = chrono lancé
static int           tirCount    = 0;
static unsigned long startTime   = 0;
static int           lastPhase   = -1;     // phase RGB courante (évite setColor à chaque loop)

void classiqueReset() {
    gameRunning = false;
    tirCount    = 0;
    startTime   = 0;
    lastPhase   = -1;
    eteindre_all_leds_tir();
    setColor(0, 0, 0);
    LOGLN("[CLASSIQUE] En attente du premier tir...");
}

// Animation victoire : chenillard rapide 3 fois + RGB vert
static void animationVictoire() {
    LOGLN("[CLASSIQUE] VICTOIRE !");
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
    classiqueReset();
}

// Animation défaite : clignotement rouge 5 fois
static void animationDefaite() {
    LOGLN("[CLASSIQUE] DEFAITE - Temps écoulé !");
    eteindre_all_leds_tir();
    for (int r = 0; r < 5; r++) {
        setColor(255, 0, 0);
        delay(150);
        setColor(0, 0, 0);
        delay(150);
    }
    delay(1000);
    classiqueReset();
}

void modeClassique() {
    // Phase d'attente : on attend le premier tir pour lancer le timer
    if (!gameRunning) {
        if (cibleToucheeDebounce()) {
            gameRunning = true;
            startTime   = millis();
            tirCount    = 1;
            lastPhase   = -1;
            digitalWrite(PIN_LED_TIR[0], HIGH);
            LOGLN("[CLASSIQUE] Premier tir ! Début du chrono !");
            LOG("[CLASSIQUE] Tir réussi ! 1/");
            LOGLN(LED_PIN_TIR_COUNT);
        }
        return;
    }

    unsigned long elapsed = millis() - startTime;

    // Vérifier le temps écoulé — déterminer la phase
    int phase = elapsed / CLASSIQUE_PHASE_DURATION;
    if (phase >= CLASSIQUE_NB_PHASES) {
        animationDefaite();
        return;
    }

    // Mettre à jour la couleur RGB si on change de phase
    if (phase != lastPhase) {
        lastPhase = phase;
        setColor(CLASSIQUE_COULEURS[phase][0], CLASSIQUE_COULEURS[phase][1], CLASSIQUE_COULEURS[phase][2]);
        LOG("[CLASSIQUE] Phase ");
        LOG(phase + 1);
        LOG("/4 - ");
        unsigned long restant =
            (CLASSIQUE_NB_PHASES - phase) * CLASSIQUE_PHASE_DURATION - (elapsed % CLASSIQUE_PHASE_DURATION);
        LOG(restant / 1000);
        LOGLN("s restantes");
    }

    // Détection de tir avec anti-rebond
    if (cibleToucheeDebounce()) {
        digitalWrite(PIN_LED_TIR[tirCount], HIGH);
        tirCount++;
        LOG("[CLASSIQUE] Tir réussi ! ");
        LOG(tirCount);
        LOG("/");
        LOGLN(LED_PIN_TIR_COUNT);

        if (tirCount >= LED_PIN_TIR_COUNT) {
            animationVictoire();
            return;
        }
    }
}
