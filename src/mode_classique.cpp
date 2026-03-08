#include "mode_classique.h"

#include "config.h"
#include "detection.h"
#include "leds.h"

// Couleurs du timer : vert → jaune → orange → rouge, 10s chacune (40s total)
const unsigned long CLASSIQUE_PHASE_DURATION                   = 10000;
const int           CLASSIQUE_NB_PHASES                        = 4;
const uint8_t       CLASSIQUE_COULEURS[CLASSIQUE_NB_PHASES][3] = {
    {0, 255, 0},    // vert
    {255, 240, 0},  // jaune vif
    {255, 80, 0},   // orange foncé
    {255, 0, 0}     // rouge
};

// Paramètres du clignotement de transition
const unsigned long BLINK_DURATION = 1200;  // durée totale du clignotement (ms)
const unsigned long BLINK_PERIOD   = 250;   // période d'un clignotement complet (ms)

// État du mode classique
static bool          gameRunning     = false;  // false = attente premier tir, true = chrono lancé
static int           tirCount        = 0;
static unsigned long startTime       = 0;
static int           lastPhase       = -1;   // phase courante
static int           prevPhase       = -1;   // phase précédente (pour le clignotement)
static unsigned long phaseChangeTime = 0;    // horodatage du dernier changement de phase
static bool          colorSet        = false;  // couleur de la nouvelle phase déjà appliquée

void classiqueReset() {
    gameRunning     = false;
    tirCount        = 0;
    startTime       = 0;
    lastPhase       = -1;
    prevPhase       = -1;
    phaseChangeTime = 0;
    colorSet        = false;
    eteindre_all_leds_tir();
    setColor(0, 0, 0);
    LOGLN("[CLASSIQUE] En attente du premier tir...");
}

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

    classiqueReset();
}

// Animation défaite : clignotement rouge 5 fois
static void animationDefaite() {
    setColor(0, 0, 0);
    eteindre_all_leds_tir();

    delay(250);

    for (int x = 0; x < 10; x++) {
        for (int x = tirCount; x < LED_PIN_TIR_COUNT; x++) {
            digitalWrite(PIN_LED_TIR[x], HIGH);
            setColor(255, 0, 0);
        }
        delay(100);
        for (int x = tirCount; x < LED_PIN_TIR_COUNT; x++) {
            digitalWrite(PIN_LED_TIR[x], LOW);
            setColor(0, 0, 0);
        }
        delay(100);
    }

    for (int x = tirCount; x < LED_PIN_TIR_COUNT; x++) {
        digitalWrite(PIN_LED_TIR[x], HIGH);
        setColor(255, 0, 0);
    }

    delay(2000);
    classiqueReset();
}

bool classiqueEnAttente() {
    return !gameRunning;
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

    // Détecter changement de phase
    if (phase != lastPhase) {
        prevPhase       = lastPhase;
        lastPhase       = phase;
        phaseChangeTime = millis();
        colorSet        = false;

        LOG("[CLASSIQUE] Phase ");
        LOG(phase + 1);
        LOG("/4 - ");
        unsigned long restant =
            (CLASSIQUE_NB_PHASES - phase) * CLASSIQUE_PHASE_DURATION - (elapsed % CLASSIQUE_PHASE_DURATION);
        LOG(restant / 1000);
        LOGLN("s restantes");
    }

    // Clignotement non-bloquant avec la couleur précédente
    bool inBlinkWindow = (prevPhase >= 0) && (millis() - phaseChangeTime < BLINK_DURATION);
    if (inBlinkWindow) {
        if ((millis() % BLINK_PERIOD) < (BLINK_PERIOD / 2)) {
            setColor(CLASSIQUE_COULEURS[prevPhase][0], CLASSIQUE_COULEURS[prevPhase][1], CLASSIQUE_COULEURS[prevPhase][2]);
        } else {
            setColor(0, 0, 0);
        }
    } else if (!colorSet) {
        setColor(CLASSIQUE_COULEURS[lastPhase][0], CLASSIQUE_COULEURS[lastPhase][1], CLASSIQUE_COULEURS[lastPhase][2]);
        colorSet = true;
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
