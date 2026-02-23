#pragma once

// Mode courant
extern int currentMode;

// Vérifie le bouton et gère la sélection de mode (clignotement 3s)
// Retourne true si on est en mode sélection (le jeu ne doit pas tourner)
bool gererSelectionMode();

// Exécute la fonction du mode courant
void executerModeCourant();
