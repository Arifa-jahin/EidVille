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

// mosque: two minarets, main hall, green dome, crescents, door, arched windows
void drawMosque() {
    float wall = night ? 0.70f : 0.95f;

    setColor(wall, wall * 0.96f, wall * 0.86f);      // minarets
    rect(300, 580, 318, 725);
    rect(482, 580, 500, 725);
    setColor(0.10f, 0.55f, 0.45f);
    halfCircle(309, 725, 12);
    halfCircle(491, 725, 12);
    rect(298, 690, 320, 696);                         // minaret balconies
    rect(480, 690, 502, 696);

    setColor(wall, wall * 0.96f, wall * 0.86f);      // main hall
    rect(325, 580, 475, 665);
    setColor(0.10f, 0.55f, 0.45f);
    rect(320, 660, 480, 668);

    setColor(0.12f, 0.62f, 0.50f);                    // big dome
    halfCircle(400, 668, 52);

    setColor(1.0f, 0.85f, 0.25f);                     // crescents on top
    rect(399, 720, 401, 732);
    crescent(400, 742, 9);
    crescent(309, 746, 5);
    crescent(491, 746, 5);

    setColor(0.40f, 0.22f, 0.10f);                    // door
    rect(386, 580, 414, 610);
    halfCircle(400, 610, 14);

    if (night) setColor(1.0f, 0.88f, 0.40f);          // windows glow at night
    else       setColor(0.55f, 0.80f, 0.95f);
    rect(342, 615, 358, 640);  halfCircle(350, 640, 8);
    rect(442, 615, 458, 640);  halfCircle(450, 640, 8);
}

// Eidgah: open prayer ground in front of the mosque
void drawEidgah() {
    if (night) setColor(0.42f, 0.38f, 0.30f);
    else       setColor(0.93f, 0.86f, 0.66f);
    rect(280, 472, 520, 580);
    setColor(0.85f, 0.75f, 0.50f);                    // prayer rows
    for (int i = 0; i < 4; i++) rect(295, 490 + i * 20, 505, 492 + i * 20);
}

// sky, ground and everything behind the roads
void drawBackground() {
    drawSkyAndGround();
    drawEidgah();
    drawMosque();
    // TODO (Ijma): people, mela, lanterns ...
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