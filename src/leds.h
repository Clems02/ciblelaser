#pragma once
#include <Arduino.h>

// Contrôle de la LED RGB (logique inversée : 255 = éteint)
void setColor(uint8_t r, uint8_t g, uint8_t b);

// Éteindre toutes les LEDs TIR
void eteindre_all_leds_tir();

// Initialisation des LEDs (TIR + RGB)
void setup_leds();

// Animation comète non-bloquante (utilisée pendant la calibration)
void animationCharging();
