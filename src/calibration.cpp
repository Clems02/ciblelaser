#include "calibration.h"
#include "config.h"
#include "leds.h"

int averageBrightness = 0;

int calculateAverageBrightness(int numSamples, int delayBetweenSamples) {
    int           totalBrightness = 0;
    int           samplesTaken    = 0;
    unsigned long lastSampleTime  = 0;

    // Première mesure immédiate
    totalBrightness += analogRead(PIN_PHOTO_RESISTANCE);
    samplesTaken++;
    lastSampleTime = millis();

    // Mesures suivantes espacées de delayBetweenSamples, animation en continu
    while (samplesTaken < numSamples) {
        animationCharging();

        if (millis() - lastSampleTime >= (unsigned long) delayBetweenSamples) {
            totalBrightness += analogRead(PIN_PHOTO_RESISTANCE);
            samplesTaken++;
            lastSampleTime = millis();
        }
    }

    // Éteindre les LEDs TIR et libérer les canaux LEDC
    // Après ledcDetach, il faut reconfigurer en GPIO OUTPUT pour que digitalWrite fonctionne
    for (int i = 0; i < LED_PIN_TIR_COUNT; i++) {
        ledcDetach(PIN_LED_TIR[i]);
        pinMode(PIN_LED_TIR[i], OUTPUT);
        digitalWrite(PIN_LED_TIR[i], LOW);
    }

    return totalBrightness / numSamples;
}
