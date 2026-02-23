#include "leds.h"

#include "config.h"

void setColor(uint8_t r, uint8_t g, uint8_t b) {
    analogWrite(PIN_RGB_RED, 255 - (uint8_t) (r * LED_RGB_INTENSITY));
    analogWrite(PIN_RGB_GREEN, 255 - (uint8_t) (g * LED_RGB_INTENSITY));
    analogWrite(PIN_RGB_BLUE, 255 - (uint8_t) (b * LED_RGB_INTENSITY));
}

void eteindre_all_leds_tir() {
    for (int i = 0; i < LED_PIN_TIR_COUNT; i++) {
        digitalWrite(PIN_LED_TIR[i], LOW);
    }
}

void setup_leds() {
    // setup des leds tir
    for (int i = 0; i < LED_PIN_TIR_COUNT; i++) {
        pinMode(PIN_LED_TIR[i], OUTPUT);
        digitalWrite(PIN_LED_TIR[i], LOW);
        // Désactiver les pull-ups internes (GPIO 8, 9 notamment) pour éviter le courant résiduel
        gpio_pullup_dis((gpio_num_t) PIN_LED_TIR[i]);
        gpio_pulldown_dis((gpio_num_t) PIN_LED_TIR[i]);
    }

    // setup de la led rgb (logique inversée : HIGH = éteint)
    digitalWrite(PIN_RGB_RED, HIGH);
    pinMode(PIN_RGB_RED, OUTPUT);
    digitalWrite(PIN_RGB_RED, HIGH);
    gpio_pullup_dis((gpio_num_t) PIN_RGB_RED);
    gpio_pulldown_dis((gpio_num_t) PIN_RGB_RED);
    pinMode(PIN_RGB_GREEN, OUTPUT);
    digitalWrite(PIN_RGB_GREEN, HIGH);
    gpio_pullup_dis((gpio_num_t) PIN_RGB_GREEN);
    gpio_pulldown_dis((gpio_num_t) PIN_RGB_GREEN);
    pinMode(PIN_RGB_BLUE, OUTPUT);
    digitalWrite(PIN_RGB_BLUE, HIGH);
    gpio_pullup_dis((gpio_num_t) PIN_RGB_BLUE);
    gpio_pulldown_dis((gpio_num_t) PIN_RGB_BLUE);
}

void animationCharging() {
    static float         t            = 0.0;
    static int           direction    = 1;
    static unsigned long lastAnimTime = 0;
    const float          speed        = 0.015;

    if (millis() - lastAnimTime < 15)
        return;
    lastAnimTime = millis();

    float position = t * (LED_PIN_TIR_COUNT - 1);

    for (int i = 0; i < LED_PIN_TIR_COUNT; i++) {
        float diff = position - i;

        // Comète : traînée douce derrière, coupure nette devant
        bool  isBehind = (direction == 1) ? (diff > 0) : (diff < 0);
        float decay    = isBehind ? 1.8 : 4.0;

        float intensity = exp(-fabs(diff) * decay);
        int   pwm       = constrain(intensity * 255, 0, 255);
        analogWrite(PIN_LED_TIR[i], pwm);
    }

    t += speed * direction;

    if (t >= 1.0) {
        t         = 1.0;
        direction = -1;
    } else if (t <= 0.0) {
        t         = 0.0;
        direction = 1;
    }
}
