#pragma once
#include <Arduino.h>

// Anti-rebond pour la détection laser
const unsigned long HIT_COOLDOWN = 500;

// Vérifie si la cible est touchée (luminosité au-dessus du seuil)
bool cibleTouchee();

// Vérifie si la cible est touchée avec anti-rebond
bool cibleToucheeDebounce();
