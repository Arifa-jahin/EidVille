// ============================================================
//  scene.cpp  -  Eid town graphics and animation
//  Owner: Ijma
// ============================================================
#include "common.h"
extern int giftsLeft;                  // defined in route.cpp (gifts not delivered yet)



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
    if (!night) {                                              // clouds and birds drift across the sky
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
    float tr[12][3] = {{35,220,1.0f},{35,450,1.1f},{180,240,1.1f},{170,300,0.8f},{325,300,0.8f},
                       {650,280,1.1f},{600,220,0.8f},{200,440,1.0f},{625,445,0.9f},{230,30,1.0f},
                       {560,30,1.1f},{770,40,0.9f}};
    for (int i = 0; i < 12; i++) tree(tr[i][0], tr[i][1], tr[i][2]);
    float lp[6][2] = {{170,393},{380,393},{600,393},{170,52},{380,52},{600,52}};
    for (int i = 0; i < 6; i++) streetLamp(lp[i][0], lp[i][1]);
    minaretLights();
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

// sky, ground and everything behind the roads
void drawBackground() {
    drawSkyAndGround();
    drawSkyLife();
    drawEidgah();
    drawMosque();
    drawDecorations();
}

// things drawn on top of the roads and houses
void drawForeground() {
    drawGiftBoy(walkerX, walkerY, cos(walkerAngle), walking, giftsLeft > 0);
    drawPopup();
}

void sceneKey(unsigned char key) {
    if (key == 'n' || key == 'N') night = !night;   // day / night
}

void sceneUpdate() {
    if (giftsLeft < lastGiftsLeft) {                // a gift was just delivered
        popupFrames = 90;
        popupX = walkerX; popupY = walkerY;
    }
    lastGiftsLeft = giftsLeft;
    if (popupFrames > 0) popupFrames--;
}