#include <Arduino.h>
#include <WiFi.h>
#include <esp_bt.h>

#include "calibration.h"
#include "config.h"
#include "leds.h"
#include "mode_selection.h"

void setup() {
    Serial.begin(115200);
    setCpuFrequencyMhz(40);

    // Désactiver les radios inutilisées pour économiser l'énergie
    WiFi.mode(WIFI_OFF);
    esp_bt_controller_disable();

    setup_leds();
    pinMode(PIN_PHOTO_RESISTANCE, INPUT);
    pinMode(PIN_BOUTON, INPUT_PULLUP);

    averageBrightness = calculateAverageBrightness(SAMPLES_COUNT, SAMPLE_DELAY);
}

void loop() {
    if (gererSelectionMode()) {
        delay(2);
        return;
    }

    executerModeCourant();
    delay(2);
}
