// ============================================================
//  scene.cpp  -  Eid town graphics and animation
//  Owner: Ijma
// ============================================================
#include "common.h"
extern int giftsLeft;
#ifdef _WIN32
#include <mmsystem.h>                  // mciSendString: play the Eid song (needs -lwinmm)
#endif                  // defined in route.cpp (gifts not delivered yet)

bool night = false;
bool raining = false;                  // W key: rain
bool music = false;                    // M key: Eid song
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
// it is the part of a circle that is NOT covered by a second, shifted circle
void crescent(float cx, float cy, float r) {
    // angles (degree) where the two circles meet, worked out once:
    // outer circle from 78.6 to 324.5, inner circle from 102.4 to 300.7
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

// ---------- small helpers for this file only ----------
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
}

// sky, ground, sun (day) or moon and stars (night)
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
    } else if (!raining) {
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

// ------------------------------------------------------------
//  TOWN DECORATION: clouds, birds, trees, street lamps, lights
// ------------------------------------------------------------
static void cloud(float x, float y, float s) {
    float c[6][3] = {{0,0,16},{18,5,19},{38,0,15},{-17,-2,13},{9,8,15},{28,-5,13}};
    glColor4f(0.88f, 0.92f, 0.97f, 0.95f);
    for (int i = 0; i < 6; i++) circle(x + c[i][0] * s, y + c[i][1] * s - 2 * s, c[i][2] * s);
    glColor4f(1, 1, 1, 0.95f);
    for (int i = 0; i < 6; i++) circle(x + c[i][0] * s, y + c[i][1] * s + 2 * s, c[i][2] * s * 0.9f);
}

static void bird(float x, float y, float flap) {
    setColor(0.15f, 0.15f, 0.20f);
    glLineWidth(2);
    glBegin(GL_LINE_STRIP);
    glVertex2f(x - 8, y + flap); glVertex2f(x - 3, y + 2); glVertex2f(x, y);
    glVertex2f(x + 3, y + 2);    glVertex2f(x + 8, y + flap);
    glEnd();
    glLineWidth(1);
}

static void tree(float x, float y, float s) {
    float k = night ? 0.55f : 1.0f;
    shadow(x + 4, y, 16 * s, 4 * s);
    setColor(0.42f * k, 0.27f * k, 0.15f * k);
    rect(x - 3 * s, y, x + 3 * s, y + 18 * s);
    float sway = 1.5f * sin(frameNo * 0.03f + x);             // leaves move in the wind
    setColor(0.13f * k, 0.42f * k, 0.20f * k);
    circle(x + sway, y + 30 * s, 15 * s); circle(x - 10 * s + sway, y + 22 * s, 11 * s); circle(x + 10 * s + sway, y + 22 * s, 11 * s);
    setColor(0.22f * k, 0.58f * k, 0.28f * k);
    circle(x - 4 * s + sway, y + 33 * s, 9 * s); circle(x - 12 * s + sway, y + 24 * s, 6 * s);
}

static void streetLamp(float x, float y) {
    setColor(0.20f, 0.20f, 0.24f);
    rect(x - 1.5f, y, x + 1.5f, y + 34);
    rect(x - 1.5f, y + 32, x + 8, y + 35);
    if (night) {
        glow(x + 8, y + 30, 34, 1.0f, 0.85f, 0.45f, 0.45f);
        glow(x + 8, y, 22, 1.0f, 0.85f, 0.45f, 0.20f);         // light on the ground
        setColor(1.0f, 0.92f, 0.60f);
    } else setColor(0.85f, 0.85f, 0.80f);
    circle(x + 8, y + 31, 3.5f);
}

// coloured lights hanging between the two minarets, blinking at night
static void minaretLights() {
    float cc[4][3] = {{1,0.35f,0.35f},{0.4f,1,0.5f},{0.4f,0.7f,1},{1,0.85f,0.3f}};
    setColor(0.2f, 0.2f, 0.2f);
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i <= 20; i++) { float t = i / 20.0f; glVertex2f(309 + 182 * t, 694 - 40 * 4 * t * (1 - t)); }
    glEnd();
    for (int i = 0; i <= 14; i++) {
        float t = i / 14.0f, x = 309 + 182 * t, y = 692 - 40 * 4 * t * (1 - t);
        bool on = !night || ((frameNo / 12 + i) % 3 != 0);
        float* c = cc[i % 4];
        if (night && on) glow(x, y, 9, c[0], c[1], c[2], 0.6f);
        glColor4f(c[0], c[1], c[2], on ? 1.0f : 0.35f);
        circle(x, y, 2.6f);
    }
}

// clouds and birds (drawn behind the mosque)
void drawSkyLife() {
    if (!night && !raining) {                                  // kites flying in the sky
        float kx[2] = {120, 690}, ky[2] = {700, 760};
        float kc[2][3] = {{0.95f, 0.30f, 0.35f}, {0.30f, 0.60f, 0.95f}};
        for (int i = 0; i < 2; i++) {
            float x = kx[i] + 12 * sin(frameNo * 0.02f + i * 2), y = ky[i] + 8 * sin(frameNo * 0.03f + i);
            glColor4f(0.3f, 0.3f, 0.3f, 0.6f);                 // string down to the ground
            glBegin(GL_LINE_STRIP);
            for (int k = 0; k <= 10; k++) { float t = k / 10.0f; glVertex2f(x + (kx[i] - 60 - x) * t, y - (y - 560) * t + 20 * sin(t * PI)); }
            glEnd();
            glPushMatrix();
            glTranslatef(x, y, 0);
            glRotatef(10 * sin(frameNo * 0.05f + i), 0, 0, 1);   // kite swings in the wind
            setColor(kc[i][0], kc[i][1], kc[i][2]);
            glBegin(GL_QUADS); glVertex2f(0, -16); glVertex2f(12, 0); glVertex2f(0, 18); glVertex2f(-12, 0); glEnd();
            setColor(1, 1, 1); glBegin(GL_LINES); glVertex2f(0, -16); glVertex2f(0, 18); glVertex2f(-12, 0); glVertex2f(12, 0); glEnd();
            for (int k = 0; k < 4; k++) {                      // tail
                float tx = -3 + 3 * sin(frameNo * 0.15f + k), ty = -20 - k * 8;
                setColor(1.0f, 0.85f, 0.25f); triangle(tx - 3, ty, tx + 3, ty, tx, ty - 5);
            }
            glPopMatrix();
        }
    }
    if (raining) {                                             // dark rain clouds
        for (int i = 0; i < 6; i++) {
            float x = fmod(frameNo * 0.3f + i * 150, 950) - 80, y = 740 + (i % 3) * 18;
            glColor4f(0.45f, 0.47f, 0.52f, 0.95f);
            circle(x, y, 26); circle(x + 26, y + 6, 30); circle(x + 52, y, 24); circle(x + 20, y - 10, 22);
        }
    }
    if (!night && !raining) {                                  // clouds and birds drift across the sky
        cloud(fmod(frameNo * 0.20f + 60, 950) - 80, 735, 1.0f);
        cloud(fmod(frameNo * 0.12f + 520, 950) - 80, 760, 0.75f);
        cloud(fmod(frameNo * 0.16f + 300, 950) - 80, 690, 0.6f);
        for (int i = 0; i < 4; i++) {
            float bx = fmod(frameNo * 0.9f + i * 28, 950) - 80;
            bird(bx, 650 + i * 7 + 5 * sin(frameNo * 0.05f + i), 5 * sin(frameNo * 0.35f + i));
        }
    }
}

// trees, street lamps and lights (drawn in front of the ground)
void drawDecorations() {
    float tr[12][3] = {{35,220,1.0f},{35,450,1.1f},{35,330,0.9f},{770,250,0.8f},{325,300,0.8f},
                       {665,300,1.0f},{770,310,0.7f},{200,440,1.0f},{625,445,0.9f},{230,30,1.0f},
                       {560,30,1.1f},{770,40,0.9f}};
    for (int i = 0; i < 12; i++) tree(tr[i][0], tr[i][1], tr[i][2]);
    float lp[6][2] = {{170,393},{380,393},{600,393},{170,52},{380,52},{600,52}};
    for (int i = 0; i < 6; i++) streetLamp(lp[i][0], lp[i][1]);
    minaretLights();
}

// ------------------------------------------------------------
//  EID JAMAAT: people pray in rows, then hug each other (kolakuli)
// ------------------------------------------------------------
// one person seen from behind (facing the mosque). pose: 0 stand, 1 ruku, 2 sit, 3 sujud
static void prayingPerson(float x, float y, int pose, float k) {
    setColor(0.96f * k, 0.96f * k, 0.94f * k);
    if (pose == 0) {                                       // standing (qiyam)
        rect(x - 5, y, x + 5, y + 20);
        setColor(0.85f * k, 0.62f * k, 0.45f * k); circle(x, y + 24, 4.5f);
        setColor(1.0f * k, 1.0f * k, 0.98f * k); halfCircle(x, y + 25, 4.6f);
    } else if (pose == 1) {                                // bowing (ruku): back is flat
        rect(x - 5, y, x + 5, y + 11);
        rect(x - 6, y + 11, x + 6, y + 15);
        setColor(1.0f * k, 1.0f * k, 0.98f * k); halfCircle(x, y + 14, 4);
    } else if (pose == 2) {                                // sitting
        rect(x - 6, y, x + 6, y + 13);
        setColor(0.85f * k, 0.62f * k, 0.45f * k); circle(x, y + 17, 4.5f);
        setColor(1.0f * k, 1.0f * k, 0.98f * k); halfCircle(x, y + 18, 4.6f);
    } else {                                               // prostration (sujud): low and round
        glBegin(GL_TRIANGLE_FAN); glVertex2f(x, y + 2);
        for (int i = 0; i <= 20; i++) { float t = PI * i / 20; glVertex2f(x + 7 * cos(t), y + 8 * sin(t)); }
        glEnd();
    }
}

// two people hugging, seen from the front
static void huggingPair(float x, float y, float k, float c1, float c2) {
    float lean = 1.5f * sin(frameNo * 0.08f + x);              // gentle swaying while hugging
    for (int p = 0; p < 2; p++) {
        float px = x + (p ? 6 : -6) + lean, col = p ? c2 : c1;
        glColor4f(0.96f * k, (0.96f - col) * k, (0.94f - col * 1.6f) * k, 1);
        rect(px - 5, y, px + 5, y + 20);
        setColor(0.85f * k, 0.62f * k, 0.45f * k); circle(px, y + 24, 4.5f);
        setColor(1.0f * k, 1.0f * k, 0.98f * k); halfCircle(px, y + 25, 4.6f);
    }
    glColor4f(0.90f * k, 0.88f * k, 0.86f * k, 1);             // arms around each other
    thickLine(x - 8 + lean, y + 17, x + 9 + lean, y + 15, 3);
    thickLine(x + 8 + lean, y + 13, x - 9 + lean, y + 11, 3);
}

// the Eid prayer happens only in the day, in the first 15 s of every 25 s cycle
bool prayerTime() { return !night && frameNo % 1500 < 900; }

void drawJamaat() {
    float k = night ? 0.65f : 1.0f;
    int t = frameNo % 1500;                                    // one prayer + greeting cycle = 25 s
    if (night) return;                                         // no Eid prayer at night: Eidgah is empty
    if (prayerTime()) {
        int pose = 0;                                          // the steps of the prayer
        if (t >= 300 && t < 420) pose = 1;
        else if ((t >= 480 && t < 600) || (t >= 660 && t < 780)) pose = 3;
        else if ((t >= 600 && t < 660) || t >= 780) pose = 2;
        prayingPerson(400, 556, pose, k);                      // imam in front
        for (int row = 0; row < 3; row++)
            for (int i = 0; i < 8; i++)
                prayingPerson(305 + i * 27 + (row % 2) * 6, 482 + (2 - row) * 22, pose, k);
    } else {                                                   // after the prayer: kolakuli
        for (int i = 0; i < 5; i++) huggingPair(318 + i * 42, 530, k, (i % 2) * 0.25f, 0.1f);
        for (int i = 0; i < 4; i++) huggingPair(340 + i * 45, 484, k, 0.1f, (i % 2) * 0.3f);
        setColor(0.10f, 0.45f, 0.25f);
        drawTextCentered(400, 562, "Eid Mubarak!", GLUT_BITMAP_HELVETICA_12);
    }
}

// ------------------------------------------------------------
//  NIGHT FIREWORKS, RAIN, EID SONG
// ------------------------------------------------------------
static void firework(float cx, float cy, int start, float r, float g, float b) {
    float p = fmod(frameNo + start, 200) / 200.0f;
    if (p < 0.25f) {                                           // rocket going up
        float y = 580 + (cy - 580) * (p / 0.25f);
        setColor(1, 0.9f, 0.6f); circle(cx, y, 2);
        return;
    }
    float q = (p - 0.25f) / 0.75f, rad = 55 * sqrt(q), fall = 25 * q * q, a = 1 - q;
    glow(cx, cy - fall, rad * 1.2f, r, g, b, 0.22f * a);
    glLineWidth(2); glBegin(GL_LINES);                         // sparks fly out in a circle
    for (int i = 0; i < 24; i++) {
        float ang = i * 2 * PI / 24;
        glColor4f(r, g, b, 0); glVertex2f(cx + rad * 0.5f * cos(ang), cy + rad * 0.5f * sin(ang) - fall * 0.5f);
        glColor4f(r, g, b, a); glVertex2f(cx + rad * cos(ang), cy + rad * sin(ang) - fall);
    }
    glEnd(); glLineWidth(1);
}

void drawFireworks() {
    if (!night || raining) return;
    firework(120, 720, 0, 1.0f, 0.40f, 0.50f);
    firework(560, 760, 70, 0.40f, 0.80f, 1.0f);
    firework(720, 700, 140, 1.0f, 0.85f, 0.30f);
    firework(230, 770, 110, 0.60f, 1.0f, 0.50f);
}

void drawRain() {
    if (!raining) return;
    glColor4f(0.15f, 0.18f, 0.25f, 0.25f);                     // darker, wet light
    rect(0, 0, 800, WIN_H);
    glLineWidth(1.5f);
    glBegin(GL_LINES);
    for (int i = 0; i < 220; i++) {
        float x = fmod(i * 73.3f + frameNo * 2.0f, 830) - 15;
        float y = WIN_H - fmod(i * 41.7f + frameNo * 11.0f, WIN_H);
        glColor4f(0.75f, 0.82f, 0.95f, 0.15f); glVertex2f(x + 4, y + 16);
        glColor4f(0.85f, 0.90f, 1.00f, 0.75f); glVertex2f(x, y);
    }
    glEnd();
    glLineWidth(1);
    for (int i = 0; i < 25; i++) {                             // small splashes on the ground
        float x = fmod(i * 131.0f, 790) + 5, y = fmod(i * 97.0f, 560) + 10;
        float r = fmod(frameNo * 0.3f + i * 3, 8);
        glColor4f(0.85f, 0.90f, 1.0f, 0.5f * (1 - r / 8));
        glBegin(GL_LINE_LOOP); for (int k = 0; k < 16; k++) { float a = 2 * PI * k / 16; glVertex2f(x + r * cos(a), y + r * 0.35f * sin(a)); } glEnd();
    }
}

static void musicNote(float x, float y, float a, bool twin) {
    glColor4f(1.0f, 0.85f, 0.3f, a);
    glBegin(GL_TRIANGLE_FAN); glVertex2f(x, y);
    for (int i = 0; i <= 16; i++) { float t = 2 * PI * i / 16; glVertex2f(x + 5 * cos(t), y + 3.6f * sin(t)); }
    glEnd();
    rect(x + 3.5f, y, x + 5, y + 18);
    if (twin) {
        glBegin(GL_TRIANGLE_FAN); glVertex2f(x + 14, y + 4);
        for (int i = 0; i <= 16; i++) { float t = 2 * PI * i / 16; glVertex2f(x + 14 + 5 * cos(t), y + 4 + 3.6f * sin(t)); }
        glEnd();
        rect(x + 17.5f, y + 4, x + 19, y + 22); rect(x + 3.5f, y + 16, x + 19, y + 19);
    } else {
        triangle(x + 5, y + 18, x + 5, y + 12, x + 11, y + 10);
    }
}

void drawMusic() {
    if (!music) return;
    if (prayerTime()) {                                        // respect the prayer: song waits
        glColor4f(0.08f, 0.10f, 0.18f, 0.8f); rect(10, 770, 290, 792);
        setColor(0.85f, 0.88f, 0.95f); drawText(18, 776, "Song paused - Eid prayer in progress", GLUT_BITMAP_HELVETICA_12);
        return;
    }
    for (int i = 0; i < 8; i++) {                              // notes float up from the mela
        float p = fmod(frameNo * 0.004f + i / 8.0f, 1.0f);
        float x = 180 + (i % 2 ? 1 : -1) * (20 + 70 * p) + 12 * sin(p * 10 + i);
        float y = 340 + 160 * p;
        musicNote(x, y, 1 - p, i % 3 == 0);
    }
    glColor4f(0.08f, 0.10f, 0.18f, 0.8f); rect(10, 770, 175, 792);
    setColor(1.0f, 0.85f, 0.3f); drawText(18, 776, "Eid song playing  (M)", GLUT_BITMAP_HELVETICA_12);
}

bool songPaused = false;

void toggleMusic() {
    music = !music;
    songPaused = false;
#ifdef _WIN32
    if (music) {
        mciSendStringA("open \"eid_song.mp3\" type mpegvideo alias eidsong", NULL, 0, NULL);
        mciSendStringA("play eidsong repeat", NULL, 0, NULL);
    } else {
        mciSendStringA("close eidsong", NULL, 0, NULL);
    }
#endif
}

// pause the song while the Eid prayer is going on, play again after it
void updateMusic() {
    if (!music) return;
    bool shouldPause = prayerTime();
    if (shouldPause == songPaused) return;
    songPaused = shouldPause;
#ifdef _WIN32
    mciSendStringA(songPaused ? "pause eidsong" : "resume eidsong", NULL, 0, NULL);
#endif
}

// ------------------------------------------------------------
//  GIFT CARRIER: a boy in panjabi and tupi carrying a gift box
//  uses translate + scale (face left or right) + rotate (arms and legs)
// ------------------------------------------------------------
static void limb(float x, float y, float angle, float len, float w) {
    glPushMatrix();
    glTranslatef(x, y, 0);
    glRotatef(angle, 0, 0, 1);                                 // swing around the joint
    rect(-w / 2, -len, w / 2, 0);
    glPopMatrix();
}

void drawGiftBoy(float x, float y, float dirX, bool moving, bool hasGift) {
    float swing = moving ? 28 * sin(frameNo * 0.35f) : 0;      // walking animation
    float bob = moving ? fabs(sin(frameNo * 0.35f)) * 1.5f : 0;
    glPushMatrix();
    glTranslatef(x, y + bob, 0);
    if (dirX < 0) glScalef(-1, 1, 1);                          // turn to face left
    glScalef(0.75f, 0.75f, 1);

    glColor4f(0, 0, 0, 0.25f);                                 // shadow
    glBegin(GL_TRIANGLE_FAN); glVertex2f(0, -bob);
    for (int i = 0; i <= 20; i++) { float t = 2 * PI * i / 20; glVertex2f(13 * cos(t), -bob + 3.5f * sin(t)); }
    glEnd();

    setColor(0.92f, 0.92f, 0.90f);                             // legs (pajama)
    limb(-2, 26, swing, 24, 6);
    limb(3, 26, -swing, 24, 6);
    setColor(0.25f, 0.15f, 0.10f);                             // sandals
    rect(-5 + 12 * sin(swing * PI / 180), 0, 3 + 12 * sin(swing * PI / 180), 3);

    setColor(0.40f, 0.62f, 0.95f);                             // blue panjabi
    glBegin(GL_QUADS);
    glVertex2f(-10, 12); glVertex2f(10, 12); glVertex2f(8, 46); glVertex2f(-8, 46);
    glEnd();
    setColor(0.33f, 0.52f, 0.82f); rect(3, 12, 10, 46);        // shaded side
    setColor(1.0f, 0.85f, 0.35f);                              // buttons
    circle(0, 42, 1.1f); circle(0, 38, 1.1f); circle(0, 34, 1.1f);

    setColor(0.36f, 0.56f, 0.88f);                             // back arm
    limb(-6, 44, -swing * 0.8f, 20, 5);

    setColor(0.80f, 0.58f, 0.42f);                             // head
    rect(-2, 46, 2, 50);
    circle(0, 57, 8);
    setColor(0.08f, 0.06f, 0.05f); halfCircle(0, 59, 8);       // hair
    setColor(0.98f, 0.98f, 0.96f); halfCircle(0, 61, 8.2f);    // tupi
    setColor(0.85f, 0.85f, 0.82f); rect(-8.2f, 61, 8.2f, 62);
    setColor(0.08f, 0.06f, 0.05f); circle(4, 57, 1.2f);        // eye
    setColor(0.60f, 0.25f, 0.20f); rect(3, 52, 7, 53);         // smile

    if (hasGift) {                                             // arm forward holding the gift box
        setColor(0.40f, 0.62f, 0.95f); limb(5, 44, 70, 14, 5);
        setColor(0.90f, 0.20f, 0.30f); rect(12, 30, 28, 44);
        setColor(1.0f, 0.85f, 0.25f); rect(19, 30, 21, 44); rect(12, 36, 28, 38);
        triangle(20, 44, 14, 49, 15, 43); triangle(20, 44, 26, 49, 25, 43);
    } else {
        setColor(0.40f, 0.62f, 0.95f); limb(5, 44, swing * 0.8f, 20, 5);
    }
    glPopMatrix();
}

// "Gift delivered!" message that floats up and fades
int   popupFrames = 0;
float popupX = 0, popupY = 0;
int   lastGiftsLeft = 0;

void drawPopup() {
    if (popupFrames <= 0) return;
    float a = popupFrames > 30 ? 1.0f : popupFrames / 30.0f;
    float rise = (90 - popupFrames) * 0.5f;
    float bx = popupX - 50, by = popupY + 45 + rise;
    if (bx > 800 - 104) bx = 800 - 104;
    if (bx < 4) bx = 4;
    glColor4f(0.08f, 0.45f, 0.25f, 0.92f * a);
    rect(bx, by, bx + 100, by + 22);
    glColor4f(1, 0.85f, 0.3f, a);
    rect(bx, by + 20, bx + 100, by + 22);
    glColor4f(1, 1, 1, a);
    drawText(bx + 10, by + 7, "Gift delivered!", GLUT_BITMAP_HELVETICA_12);
}

// ------------------------------------------------------------
//  STREET LIFE: families walking, rickshaw, food cart, fair visitors
//  (more people in the day, fewer people and more lights at night)
// ------------------------------------------------------------
// small walking person: top colour (r,g,b), kid = smaller, female = orna/scarf
static void townPerson(float x, float y, float s, float r, float g, float b,
                       bool walkingNow, float dir, bool kid, bool tupi, bool female) {
    float k = night ? 0.7f : 1.0f;
    float h = kid ? 0.75f : 1.0f;
    float swing = walkingNow ? 25 * sin(frameNo * 0.3f + x * 0.05f) : 0;
    glPushMatrix();
    glTranslatef(x, y, 0);
    if (dir < 0) glScalef(-1, 1, 1);                       // face the walking direction
    glScalef(s * h, s * h, 1);
    glColor4f(0, 0, 0, 0.2f); glBegin(GL_TRIANGLE_FAN); glVertex2f(0, 0);
    for (int i = 0; i <= 16; i++) { float t = 2 * PI * i / 16; glVertex2f(10 * cos(t), 2.5f * sin(t)); } glEnd();
    setColor(0.25f * k, 0.22f * k, 0.25f * k);             // legs
    limb(-2, 22, swing, 22, 5); limb(2, 22, -swing, 22, 5);
    setColor(r * k, g * k, b * k);                         // body (panjabi / kameez)
    glBegin(GL_QUADS);
    glVertex2f(-8, female ? 2 : 10); glVertex2f(8, female ? 2 : 10); glVertex2f(6, 40); glVertex2f(-6, 40);
    glEnd();
    setColor(r * k * 0.85f, g * k * 0.85f, b * k * 0.85f);
    limb(-5, 38, -swing * 0.7f, 17, 4); limb(5, 38, swing * 0.7f, 17, 4);   // arms swing
    setColor(0.80f * k, 0.58f * k, 0.42f * k);             // head
    circle(0, 47, 6.5f);
    if (female) { setColor(r * k * 0.8f + 0.15f, g * k * 0.7f, b * k * 0.8f); halfCircle(0, 48, 7.5f);
                  rect(-7.5f, 40, -5, 48); rect(5, 40, 7.5f, 48); }
    else { setColor(0.08f, 0.06f, 0.05f); halfCircle(0, 49, 6.7f); }
    if (tupi) { setColor(0.98f * k, 0.98f * k, 0.96f * k); halfCircle(0, 51, 6.6f); }
    setColor(0.1f, 0.08f, 0.06f); circle(3, 47, 1);         // eye
    glPopMatrix();
}

// family: father, mother and child walking together (dir = +1 right, -1 left)
static void family(float x, float y, float dir, float c) {
    townPerson(x, y, 0.85f, 0.95f, 0.95f, 0.92f, true, dir, false, true, false);
    townPerson(x + 16 * dir, y - 2, 0.82f, 0.90f, 0.35f + c, 0.50f, true, dir, false, false, true);
    townPerson(x + 30 * dir, y - 3, 0.85f, 0.30f + c, 0.60f, 0.95f, true, dir, true, true, false);
    if (!night) {                                          // the child holds a balloon
        float bx = x + 30 * dir + 6, by = y + 52 + 3 * sin(frameNo * 0.05f + x);
        setColor(0.3f, 0.3f, 0.3f); glBegin(GL_LINES); glVertex2f(x + 30 * dir + 3, y + 22); glVertex2f(bx, by - 7); glEnd();
        setColor(0.95f, 0.30f + c, 0.35f); circle(bx, by, 6);
    } else {                                               // at night the child has a sparkler
        float tx = x + 30 * dir + 8, ty = y + 32;
        glow(tx, ty, 14, 1, 0.85f, 0.4f, 0.6f);
        glBegin(GL_LINES);
        for (int i = 0; i < 8; i++) { float a = (frameNo * 37 + i * 45) % 360 * PI / 180;
            glColor4f(1, 0.95f, 0.6f, 1); glVertex2f(tx, ty); glColor4f(1, 0.6f, 0.2f, 0); glVertex2f(tx + 9 * cos(a), ty + 9 * sin(a)); }
        glEnd();
    }
}

// small cycle rickshaw with a puller, moving along the bottom of the town
static void movingRickshaw(float x, float y) {
    float k = night ? 0.7f : 1.0f, spin = frameNo * 0.15f;
    for (int w = 0; w < 2; w++) {                          // wheels turn (rotation)
        float cx = w ? x + 28 : x - 14, r = w ? 10 : 8;
        setColor(0.1f, 0.1f, 0.12f); glLineWidth(2);
        glBegin(GL_LINE_LOOP); for (int i = 0; i < 20; i++) { float t = 2 * PI * i / 20; glVertex2f(cx + r * cos(t), y + r + r * sin(t)); } glEnd();
        glBegin(GL_LINES); for (int i = 0; i < 3; i++) { float t = spin + i * PI / 3; glVertex2f(cx + r * cos(t), y + r + r * sin(t)); glVertex2f(cx - r * cos(t), y + r - r * sin(t)); } glEnd();
        glLineWidth(1);
    }
    setColor(0.85f * k, 0.25f * k, 0.45f * k); rect(x + 10, y + 12, x + 42, y + 26);   // seat box
    setColor(1.0f * k, 0.8f * k, 0.25f * k); rect(x + 10, y + 24, x + 42, y + 26);
    setColor(0.15f * k, 0.12f * k, 0.18f * k);                                        // hood
    glBegin(GL_TRIANGLE_FAN); glVertex2f(x + 40, y + 26);
    for (int i = 0; i <= 12; i++) { float t = PI * 0.5f + PI * 0.5f * i / 12; glVertex2f(x + 42 + 26 * cos(t), y + 26 + 30 * sin(t)); }
    glEnd();
    townPerson(x - 8, y + 6, 0.62f, 0.30f, 0.55f, 0.40f, true, -1, false, false, false);   // puller
}

// fuchka / chotpoti cart, with a lamp at night
static void foodCart(float x, float y) {
    float k = night ? 0.75f : 1.0f;
    if (night) glow(x + 22, y + 40, 55, 1, 0.8f, 0.4f, 0.45f);
    setColor(0.55f * k, 0.35f * k, 0.20f * k); rect(x, y + 8, x + 44, y + 30);
    setColor(0.15f, 0.15f, 0.15f); circle(x + 8, y + 6, 5); circle(x + 36, y + 6, 5);
    setColor(0.85f * k, 0.85f * k, 0.88f * k); halfCircle(x + 14, y + 30, 7); halfCircle(x + 30, y + 30, 6);   // pots
    setColor(0.30f * k, 0.30f * k, 0.32f * k); rect(x + 2, y + 30, x + 4, y + 52); rect(x + 40, y + 30, x + 42, y + 52);
    for (int i = 0; i < 4; i++) {                          // striped canopy
        if (i % 2) setColor(1 * k, 1 * k, 1 * k); else setColor(0.95f * k, 0.35f * k, 0.25f * k);
        triangle(x - 4 + i * 13, y + 52, x + 9 + i * 13, y + 52, x + 2.5f + i * 13, y + 60);
    }
    if (night) { setColor(1, 0.92f, 0.6f); circle(x + 22, y + 48, 3); }
    setColor(1, 1, 1); drawTextCentered(x + 22, y + 16, "FUCHKA", GLUT_BITMAP_HELVETICA_10);
}

// paper lanterns and lights hanging between the houses
static void houseLights(float x1, float x2, float y) {
    float cc[4][3] = {{1,0.35f,0.35f},{0.4f,1,0.5f},{0.4f,0.7f,1},{1,0.85f,0.3f}};
    setColor(0.2f, 0.2f, 0.2f);
    glBegin(GL_LINE_STRIP); for (int i = 0; i <= 20; i++) { float t = i / 20.0f; glVertex2f(x1 + (x2 - x1) * t, y - 14 * 4 * t * (1 - t)); } glEnd();
    for (int i = 1; i < 8; i++) {
        float t = i / 8.0f, lx = x1 + (x2 - x1) * t, ly = y - 14 * 4 * t * (1 - t) - 6;
        float* c = cc[i % 4];
        bool on = !night || ((frameNo / 15 + i) % 3 != 0);
        if (night && on) glow(lx, ly, 12, c[0], c[1], c[2], 0.55f);
        glColor4f(c[0], c[1], c[2], on ? 1.0f : 0.4f);
        glBegin(GL_TRIANGLE_FAN); glVertex2f(lx, ly);
        for (int k = 0; k <= 16; k++) { float a = 2 * PI * k / 16; glVertex2f(lx + 3.5f * cos(a), ly + 5 * sin(a)); } glEnd();
    }
}

void drawStreetLife() {
    // lights between the houses on the upper road
    houseLights(358, 497, 476);
    houseLights(558, 737, 476);
    // families walking along the bottom of the town (more in the day)
    int groups = night ? 2 : 4;
    for (int g = 0; g < groups; g++) {
        float dir = (g % 2) ? -1.0f : 1.0f;
        float p = fmod(frameNo * 0.45f + g * 230, 900);
        float x = dir > 0 ? p - 60 : 840 - p;
        family(x, g % 2 ? 40 : 62, dir, g * 0.12f);
    }
    movingRickshaw(860 - fmod(frameNo * 0.8f, 960), 6);
    foodCart(450, 22);
    townPerson(505, 26, 0.8f, 0.85f, 0.85f, 0.80f, false, -1, false, true, false);    // fuchka seller
    if (night) {                                           // customers at the cart at night
        townPerson(420, 24, 0.8f, 0.95f, 0.45f, 0.60f, false, 1, false, false, true);
        townPerson(434, 24, 0.8f, 0.40f, 0.65f, 0.95f, false, 1, true, true, false);
    }
    // visitors at the fair stalls
    float vc[4][3] = {{0.95f,0.50f,0.30f},{0.40f,0.70f,0.95f},{0.95f,0.95f,0.92f},{0.85f,0.40f,0.70f}};
    int visitors = night ? 3 : 4;
    float vx[4] = {111, 163, 215, 258};                    // beside the stalls, not over their names
    for (int i = 0; i < visitors; i++)
        townPerson(vx[i], 141, 0.75f, vc[i][0], vc[i][1], vc[i][2], false,
                   i % 2 ? -1 : 1, i == 1, i == 2, i == 3);
}

// sky, ground and everything behind the roads
void drawBackground() {
    drawSkyAndGround();
    drawFireworks();
    drawSkyLife();
    drawEidgah();
    drawJamaat();
    drawMosque();
    drawDecorations();
}

// things drawn on top of the roads and houses
void drawForeground() {
    drawStreetLife();
    drawGiftBoy(walkerX, walkerY, cos(walkerAngle), walking, giftsLeft > 0);
    drawPopup();
    drawRain();
    drawMusic();
}

void sceneKey(unsigned char key) {
    if (key == 'n' || key == 'N') night = !night;     // day / night
    if (key == 'w' || key == 'W') raining = !raining; // rain on / off
    if (key == 'm' || key == 'M') toggleMusic();      // Eid song on / off
}

void sceneUpdate() {
    if (giftsLeft < lastGiftsLeft) {                // a gift was just delivered
        popupFrames = 90;
        popupX = walkerX; popupY = walkerY;
    }
    lastGiftsLeft = giftsLeft;
    if (popupFrames > 0) popupFrames--;
    updateMusic();
}