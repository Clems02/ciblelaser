#include "detection.h"

#include "config.h"

extern int averageBrightness;

static unsigned long lastHitTime = 0;

bool cibleTouchee() {
    int luminosite = analogRead(PIN_PHOTO_RESISTANCE);

    return luminosite > (averageBrightness * THRESHOLD_FACTOR) && luminosite > 200;
}

bool cibleToucheeDebounce() {
    if (cibleTouchee() && (millis() - lastHitTime) > HIT_COOLDOWN) {
        lastHitTime = millis();
        return true;
    }
    return false;
}