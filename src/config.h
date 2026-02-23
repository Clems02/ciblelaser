#pragma once
#include <Arduino.h>

// --- Pins ---
const int PIN_PHOTO_RESISTANCE = A0;

const int PIN_RGB_RED   = 4;
const int PIN_RGB_GREEN = 3;
const int PIN_RGB_BLUE  = 2;

const int PIN_BOUTON = 5;

const int PIN_LED_TIR[]     = {8, 9, 10, 20, 21};
const int LED_PIN_TIR_COUNT = 5;

// --- Calibration ---
const int   SAMPLES_COUNT    = 10;
const int   SAMPLE_DELAY     = 500;
const float THRESHOLD_FACTOR = 2;

// --- LED RGB ---
const float LED_RGB_INTENSITY = 0.1;  // 10% de luminosité (0.0 = éteint, 1.0 = max)

// --- Modes de jeu ---
enum ModeJeu { MODE_CLASSIQUE = 0, MODE_DECONTRACTE, MODE_RAPIDFIRE, MODE_SURVIE, NB_MODES };

// --- Logging conditionnel (désactivé en release pour économiser l'énergie
//     et éviter le conflit UART0 sur GPIO20/21) ---
#if DEBUG_MODE
  #define LOG(x)   Serial.print(x)
  #define LOGLN(x) Serial.println(x)
#else
  #define LOG(x)
  #define LOGLN(x)
#endif
