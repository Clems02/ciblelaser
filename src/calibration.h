#pragma once

// Luminosité de référence (calibrée au démarrage)
extern int averageBrightness;

// Calibre la luminosité ambiante (bloquant, avec animation pendant la mesure)
int calculateAverageBrightness(int numSamples, int delayBetweenSamples);
