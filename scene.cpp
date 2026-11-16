// ============================================================
//  scene.cpp  -  Eid town graphics and animation
//  Owner: Ijma
// ============================================================
#include "common.h"

bool night = false;

// sky, ground and everything behind the roads
void drawBackground() {
    setColor(0.53f, 0.81f, 0.98f);     // sky
    rect(0, 580, 800, WIN_H);
    setColor(0.45f, 0.75f, 0.35f);     // ground
    rect(0, 0, 800, 580);

    // TODO (Ijma): sun / moon, mosque, Eidgah, people, mela, lanterns ...
}

// things drawn on top of the roads and houses
void drawForeground() {
    // TODO (Ijma): gift carrier (walking boy), "Gift delivered!" message
}

void sceneKey(unsigned char key) {
    (void)key;
    // TODO (Ijma): N = day / night
}

void sceneUpdate() {
    // TODO (Ijma): animations that need updating every frame
}
