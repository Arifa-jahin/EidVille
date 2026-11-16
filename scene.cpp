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

// crescent moon drawn as a real shape (works on any sky colour)
void crescent(float cx, float cy, float r) {
    float ox1 = 78.6f, ox2 = 324.5f, ix1 = 102.4f, ix2 = 300.7f;
    float icx = cx + r * 0.38f, icy = cy + r * 0.15f, ir = r * 0.85f;
    setColor(1.0f, 0.85f, 0.25f);
    glBegin(GL_TRIANGLE_STRIP);
    for (int i = 0; i <= 30; i++) {
        float s = i / 30.0f;
        float ao = (ox1 + s * (ox2 - ox1)) * PI / 180;
        float ai = (ix1 + s * (ix2 - ix1)) * PI / 180;
        glVertex2f(cx + r * cos(ao), cy + r * sin(ao));
        glVertex2f(icx + ir * cos(ai), icy + ir * sin(ai));
    }
    glEnd();
}

// rectangle whose colour changes smoothly from bottom to top (gradient)
static void gradRect(float x1, float y1, float x2, float y2,
                     float r1, float g1, float b1, float r2, float g2, float b2) {
    glBegin(GL_QUADS);
    glColor4f(r1, g1, b1, 1); glVertex2f(x1, y1); glVertex2f(x2, y1);
    glColor4f(r2, g2, b2, 1); glVertex2f(x2, y2); glVertex2f(x1, y2);
    glEnd();
}

// soft round glow (transparent circle)
static void glow(float cx, float cy, float r, float red, float g, float b, float a) {
    glBegin(GL_TRIANGLE_FAN);
    glColor4f(red, g, b, a); glVertex2f(cx, cy);
    glColor4f(red, g, b, 0);
    for (int i = 0; i <= 40; i++) {
        float t = 2 * PI * i / 40;
        glVertex2f(cx + r * cos(t), cy + r * sin(t));
    }
    glEnd();
}

// soft shadow on the ground (flat transparent ellipse)
static void shadow(float cx, float cy, float rx, float ry) {
    glColor4f(0, 0, 0, 0.18f);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx, cy);
    for (int i = 0; i <= 40; i++) {
        float t = 2 * PI * i / 40;
        glVertex2f(cx + rx * cos(t), cy + ry * sin(t));
    }
    glEnd();
}

// dome with light in the middle and darker edge (looks round)
static void shadedDome(float cx, float cy, float r) {
    glBegin(GL_TRIANGLE_FAN);
    glColor4f(0.30f, 0.78f, 0.64f, 1); glVertex2f(cx - r * 0.25f, cy + r * 0.45f);
    glColor4f(0.07f, 0.45f, 0.36f, 1);
    for (int i = 0; i <= 30; i++) {
        float t = PI * i / 30;
        glVertex2f(cx + r * cos(t), cy + r * sin(t));
    }
    glVertex2f(cx + r, cy);
    glEnd();
}// sky, ground, sun (day) or moon and stars (night)
void drawSkyAndGround() {
    // sky: deep colour at the top, light near the horizon
    if (night) { skyR = 0.06f; skyG = 0.08f; skyB = 0.22f;
                 gradRect(0, 580, 800, WIN_H, 0.12f, 0.14f, 0.32f, 0.02f, 0.03f, 0.10f); }
    else       { skyR = 0.62f; skyG = 0.83f; skyB = 0.97f;
                 gradRect(0, 580, 800, WIN_H, 0.80f, 0.91f, 0.99f, 0.33f, 0.60f, 0.92f); }

    // distant hills on the horizon
    float h1 = night ? 0.10f : 0.55f, h2 = night ? 0.16f : 0.68f;
    glColor4f(h1 * 0.8f, h1 + 0.08f, h1 * 0.9f, 1);
    glBegin(GL_TRIANGLE_STRIP);
    for (int x = 0; x <= 800; x += 20) {
        glVertex2f(x, 580);
        glVertex2f(x, 600 + 18 * sin(x * 0.012f) + 8 * sin(x * 0.031f));
    }
    glEnd();
    glColor4f(h2 * 0.75f, h2, h2 * 0.7f, 1);
    glBegin(GL_TRIANGLE_STRIP);
    for (int x = 0; x <= 800; x += 20) {
        glVertex2f(x, 580);
        glVertex2f(x, 588 + 10 * sin(x * 0.02f + 1.5f));
    }
    glEnd();

    // ground: slightly darker towards the bottom
    if (night) gradRect(0, 0, 800, 580, 0.08f, 0.19f, 0.10f, 0.14f, 0.30f, 0.16f);
    else       gradRect(0, 0, 800, 580, 0.36f, 0.64f, 0.28f, 0.50f, 0.78f, 0.38f);

    if (night) {
        float s[12][2] = {{40,660},{120,750},{200,700},{240,790},{560,790},{600,660},
                          {740,760},{770,660},{30,770},{160,620},{530,710},{710,620}};
        setColor(1, 1, 0.85f);
        for (int i = 0; i < 12; i++) star(s[i][0], s[i][1], (i % 3 == 0) ? 5 : 3);
        glow(650, 715, 70, 1.0f, 0.9f, 0.5f, 0.25f);     // moonlight
        crescent(650, 715, 28);                    // Eid moon
    } else {
        glow(650, 715, 80, 1.0f, 0.95f, 0.6f, 0.45f);    // sunshine
        setColor(1.0f, 0.86f, 0.30f);
        circle(650, 715, 28);                      // sun
    }
}

// mosque: two minarets, main hall, green dome, crescents, door, arched windows
void drawMosque() {
    float wall = night ? 0.70f : 0.95f;
    float sh = 0.86f;                                 // shade factor for the side in shadow

    if (night) glow(400, 650, 150, 1.0f, 0.85f, 0.4f, 0.18f);   // mosque lit at night
    shadow(400, 580, 120, 8);

    setColor(wall, wall * 0.96f, wall * 0.86f);      // minarets
    rect(300, 580, 318, 725);
    rect(482, 580, 500, 725);
    setColor(wall * sh, wall * 0.96f * sh, wall * 0.86f * sh);   // shaded half
    rect(310, 580, 318, 725);
    rect(492, 580, 500, 725);
    setColor(0.10f, 0.55f, 0.45f);
    halfCircle(309, 725, 12);
    halfCircle(491, 725, 12);
    rect(298, 690, 320, 696);                         // minaret balconies
    rect(480, 690, 502, 696);

    setColor(wall, wall * 0.96f, wall * 0.86f);      // main hall
    rect(325, 580, 475, 665);
    setColor(wall * sh, wall * 0.96f * sh, wall * 0.86f * sh);   // shaded right part
    rect(440, 580, 475, 665);
    setColor(0.80f * wall, 0.74f * wall, 0.60f * wall);          // base step
    rect(318, 580, 482, 586);
    setColor(0.10f, 0.55f, 0.45f);
    rect(320, 660, 480, 668);

    shadedDome(400, 668, 52);                         // big dome
    setColor(0.07f, 0.45f, 0.36f);
    rect(394, 668, 406, 672);

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
    setColor(0.78f, 0.68f, 0.45f);                    // border
    rect(280, 472, 520, 475); rect(280, 577, 520, 580);
    rect(280, 472, 283, 580); rect(517, 472, 520, 580);
    float mats[3][3] = {{0.78f,0.42f,0.40f},{0.40f,0.62f,0.50f},{0.45f,0.55f,0.75f}};
    for (int row = 0; row < 3; row++)                 // rows of prayer mats
        for (int i = 0; i < 9; i++) {
            float* c = mats[(row + i) % 3];
            float k = night ? 0.6f : 1.0f;
            setColor(c[0] * k, c[1] * k, c[2] * k);
            float x = 298 + i * 23, y = 492 + row * 26;
            rect(x, y, x + 15, y + 11);
        }
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