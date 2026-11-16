// ============================================================
//  scene.cpp  -  Eid town graphics and animation
//  Owner: Ijma
// ============================================================
#include "common.h"

bool night = false;
float skyR, skyG, skyB;                // current sky colour (used for the crescent)

// 5-point star
void star(float cx, float cy, float R) {
    float r = R * 0.45f;
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx, cy);
    for (int i = 0; i <= 10; i++) {
        float a = PI / 2 + i * PI / 5;
        float rr = (i % 2 == 0) ? R : r;
        glVertex2f(cx + rr * cos(a), cy + rr * sin(a));
    }
    glEnd();
}

// crescent = gold circle, then a sky-coloured circle shifted to the right
void crescent(float cx, float cy, float r) {
    setColor(1.0f, 0.85f, 0.25f);
    circle(cx, cy, r);
    setColor(skyR, skyG, skyB);
    circle(cx + r * 0.38f, cy + r * 0.15f, r * 0.85f);
}

// sky, ground, sun (day) or moon and stars (night)
void drawSkyAndGround() {
    if (night) { skyR = 0.05f; skyG = 0.07f; skyB = 0.20f; }
    else       { skyR = 0.53f; skyG = 0.81f; skyB = 0.98f; }
    setColor(skyR, skyG, skyB);
    rect(0, 580, 800, WIN_H);

    if (night) setColor(0.13f, 0.28f, 0.15f);
    else       setColor(0.45f, 0.75f, 0.35f);
    rect(0, 0, 800, 580);

    if (night) {
        float s[12][2] = {{40,660},{120,750},{200,700},{240,790},{560,790},{600,660},
                          {740,760},{770,660},{30,770},{160,620},{530,710},{710,620}};
        setColor(1, 1, 0.85f);
        for (int i = 0; i < 12; i++) star(s[i][0], s[i][1], (i % 3 == 0) ? 5 : 3);
        crescent(650, 715, 28);                    // Eid moon
    } else {
        setColor(1.0f, 0.85f, 0.2f);
        circle(650, 715, 28);                      // sun
    }
}

// sky, ground and everything behind the roads
void drawBackground() {
    drawSkyAndGround();
    // TODO (Ijma): mosque, Eidgah, people, mela, lanterns ...
}

// things drawn on top of the roads and houses
void drawForeground() {
    // TODO (Ijma): gift carrier (walking boy), "Gift delivered!" message
}

void sceneKey(unsigned char key) {
    if (key == 'n' || key == 'N') night = !night;   // day / night
}

void sceneUpdate() {
    // TODO (Ijma): animations that need updating every frame
}