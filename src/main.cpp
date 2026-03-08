#include <Arduino.h>
#include <WiFi.h>
#include <driver/gpio.h>
#include <esp_bt.h>
#include <esp_sleep.h>

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

    if (modeJeuEnAttente()) {
        // Verrouiller les GPIOs pour éviter qu'ils flottent pendant le sleep
        for (int i = 0; i < LED_PIN_TIR_COUNT; i++) {
            gpio_hold_en((gpio_num_t) PIN_LED_TIR[i]);
        }
        // Aucun jeu en cours : light sleep 50ms pour économiser l'énergie
        esp_sleep_enable_timer_wakeup(50000);
        esp_light_sleep_start();
        // Libérer les GPIOs après réveil pour permettre digitalWrite
        for (int i = 0; i < LED_PIN_TIR_COUNT; i++) {
            gpio_hold_dis((gpio_num_t) PIN_LED_TIR[i]);
        }
    } else {
        delay(2);
    }
}
