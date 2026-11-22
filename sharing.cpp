// ============================================================
//  sharing.cpp  -  Slide 3: "Eid in every home" (Sobar Eid)
//  Owner: Ijma
//  After the gifts are delivered, the windows of a building open one by one.
//  Each window shows the joy that the gifts bring. Then the camera zooms out.
// ============================================================
#include "common.h"
void star(float cx, float cy, float R);   // from scene.cpp

// =================== helpers ===================
static float clamp01(float v) { return v < 0 ? 0 : (v > 1 ? 1 : v); }
static float mix(float a, float b, float t) { return a + (b - a) * t; }
static void col(float r, float g, float b, float a = 1) { glColor4f(r, g, b, a); }

static void grad(float x1, float y1, float x2, float y2,
                 float r1, float g1, float b1, float r2, float g2, float b2, float a1 = 1, float a2 = 1) {
    glBegin(GL_QUADS);
    glColor4f(r1, g1, b1, a1); glVertex2f(x1, y1); glVertex2f(x2, y1);
    glColor4f(r2, g2, b2, a2); glVertex2f(x2, y2); glVertex2f(x1, y2);
    glEnd();
}
static void hgrad(float x1, float y1, float x2, float y2,
                  float r1, float g1, float b1, float a1, float r2, float g2, float b2, float a2) {
    glBegin(GL_QUADS);
    glColor4f(r1, g1, b1, a1); glVertex2f(x1, y1); glVertex2f(x1, y2);
    glColor4f(r2, g2, b2, a2); glVertex2f(x2, y2); glVertex2f(x2, y1);
    glEnd();
}
static void glow(float cx, float cy, float r, float red, float g, float b, float a) {
    glBegin(GL_TRIANGLE_FAN);
    glColor4f(red, g, b, a); glVertex2f(cx, cy);
    glColor4f(red, g, b, 0);
    for (int i = 0; i <= 40; i++) { float t = 2 * PI * i / 40; glVertex2f(cx + r * cos(t), cy + r * sin(t)); }
    glEnd();
}
static void ellipse(float cx, float cy, float rx, float ry) {
    glBegin(GL_TRIANGLE_FAN); glVertex2f(cx, cy);
    for (int i = 0; i <= 40; i++) { float t = 2 * PI * i / 40; glVertex2f(cx + rx * cos(t), cy + ry * sin(t)); }
    glEnd();
}
static void arcLine(float cx, float cy, float rx, float ry, float a1, float a2) {
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i <= 24; i++) { float a = a1 + (a2 - a1) * i / 24; glVertex2f(cx + rx * cos(a), cy + ry * sin(a)); }
    glEnd();
}
static void roundRect(float x1, float y1, float x2, float y2, float r) {   // one polygon (safe with transparency)
    float cx[4] = {x2 - r, x1 + r, x1 + r, x2 - r}, cy[4] = {y2 - r, y2 - r, y1 + r, y1 + r};
    glBegin(GL_POLYGON);
    for (int c = 0; c < 4; c++)
        for (int i = 0; i <= 6; i++) { float a = (c * 90 + i * 15) * PI / 180; glVertex2f(cx[c] + r * cos(a), cy[c] + r * sin(a)); }
    glEnd();
}
static void archShape(float x1, float y1, float x2, float y2) {   // rectangle with round top
    float r = (x2 - x1) / 2;
    rect(x1, y1, x2, y2 - r); halfCircle(x1 + r, y2 - r, r);
}
static void moon(float cx, float cy, float r, float a) {
    float ox1 = 78.6f, ox2 = 324.5f, ix1 = 102.4f, ix2 = 300.7f;
    float icx = cx + r * 0.38f, icy = cy + r * 0.15f, ir = r * 0.85f;
    glColor4f(1.0f, 0.93f, 0.68f, a);
    glBegin(GL_TRIANGLE_STRIP);
    for (int i = 0; i <= 40; i++) {
        float s = i / 40.0f;
        float ao = (ox1 + s * (ox2 - ox1)) * PI / 180, ai = (ix1 + s * (ix2 - ix1)) * PI / 180;
        glVertex2f(cx + r * cos(ao), cy + r * sin(ao));
        glVertex2f(icx + ir * cos(ai), icy + ir * sin(ai));
    }
    glEnd();
}
static float rnd(int i) { return fmod(sin(i * 12.9898f) * 43758.5453f, 1.0f) * (sin(i * 12.9898f) < 0 ? -1 : 1); }

static void strokeText(float cx, float y, const char* s, float scale, float lw) {
    float w = 0;
    for (const char* c = s; *c; c++) w += glutStrokeWidth(GLUT_STROKE_ROMAN, *c);
    glPushMatrix();
    glTranslatef(cx - w * scale / 2, y, 0);
    glScalef(scale, scale, 1);                 // scaling transformation
    glLineWidth(lw);
    for (const char* c = s; *c; c++) glutStrokeCharacter(GLUT_STROKE_ROMAN, *c);
    glLineWidth(1);
    glPopMatrix();
}

// speech bubble (size fits the text) with a tail pointing at (tx,ty); (x, y) = bottom-left
static int textW(const char* t, void* font) { int w = 0; for (; *t; t++) w += glutBitmapWidth(font, *t); return w; }
static void bubble(float x, float y, float tx, float ty, const char* l1, const char* l2) {
    float w = textW(l1, GLUT_BITMAP_HELVETICA_18);
    if (l2 && textW(l2, GLUT_BITMAP_HELVETICA_12) > w) w = textW(l2, GLUT_BITMAP_HELVETICA_12);
    float x2 = x + w + 32, y2 = y + (l2 ? 56 : 38);
    col(0, 0, 0, 0.25f); roundRect(x + 3, y - 3, x2 + 3, y2 - 3, 12);
    col(1, 1, 1, 0.96f);
    roundRect(x, y, x2, y2, 12);
    float ex = tx < x ? x + 2 : x2 - 2;                    // tail starts at the side nearest the speaker
    triangle(ex, y + 10, ex, y + 28, tx, ty);
    float mx = (x + x2) / 2;
    col(0.20f, 0.12f, 0.30f);
    drawTextCentered(mx, y2 - 24, l1, GLUT_BITMAP_HELVETICA_18);
    if (l2) { col(0.80f, 0.30f, 0.20f); drawTextCentered(mx, y2 - 44, l2, GLUT_BITMAP_HELVETICA_12); }
}

// =================== people ===================
struct Look {
    float skin;                 // skin tone
    float tr, tg, tb;           // top (panjabi / kameez / shirt)
    float br, bg, bb;           // bottom (pajama / lungi)
    int   type;                 // 0 man, 1 woman, 2 boy, 3 girl
    bool  tupi, beard;
};

// arm with shoulder and elbow joints: uses glTranslatef + glRotatef
static void arm(float sx, float sy, float a1, float a2, float s, const Look& L, bool longSleeve) {
    float sk[3] = {L.skin, L.skin * 0.72f, L.skin * 0.55f};
    glPushMatrix();
    glTranslatef(sx, sy, 0);
    glRotatef(a1, 0, 0, 1);                                  // shoulder rotation
    col(L.tr * 0.88f, L.tg * 0.88f, L.tb * 0.88f);
    rect(-2.6f * s, -13 * s, 2.6f * s, 1 * s);
    circle(0, 0, 2.8f * s);
    glTranslatef(0, -13 * s, 0);
    glRotatef(a2, 0, 0, 1);                                  // elbow rotation
    if (longSleeve) col(L.tr * 0.88f, L.tg * 0.88f, L.tb * 0.88f); else col(sk[0], sk[1], sk[2]);
    rect(-2.3f * s, -11 * s, 2.3f * s, 0.5f * s);
    circle(0, 0, 2.4f * s);
    col(sk[0], sk[1], sk[2]);
    circle(0, -12 * s, 2.6f * s);                            // hand
    glPopMatrix();
}

// full person, feet at (x,y). la/lb = left arm angles, ra/rb = right arm angles (degrees)
static void human(float x, float y, float s, const Look& L,
                  float la = -8, float lb = 10, float ra = 8, float rb = -10, float bob = 0) {
    float sk[3] = {L.skin, L.skin * 0.72f, L.skin * 0.55f};
    bool child = L.type >= 2;
    float h = child ? 0.78f : 1.0f;              // children are shorter
    y += bob;
    col(0, 0, 0, 0.22f); ellipse(x, y - bob, 13 * s, 3 * s);           // shadow

    float hip = y + 30 * s * h, shoulder = y + 56 * s * h;
    // legs
    col(L.br, L.bg, L.bb);
    if (L.type == 1 || L.type == 3) {                    // long dress / frock
        float bottom = L.type == 1 ? y + 2 * s : y + 14 * s * h;
        glBegin(GL_QUADS);
        glVertex2f(x - 12 * s, bottom); glVertex2f(x + 12 * s, bottom);
        glVertex2f(x + 7 * s, hip); glVertex2f(x - 7 * s, hip); glEnd();
        if (L.type == 3) { col(sk[0], sk[1], sk[2]); rect(x - 5 * s, y + 2 * s, x - 2 * s, bottom); rect(x + 2 * s, y + 2 * s, x + 5 * s, bottom); }
    } else {
        rect(x - 6 * s, y + 3 * s, x - 0.5f * s, hip);
        rect(x + 0.5f * s, y + 3 * s, x + 6 * s, hip);
    }
    col(0.18f, 0.12f, 0.10f);                            // sandals
    ellipse(x - 4 * s, y + 2 * s, 4.5f * s, 2 * s); ellipse(x + 4 * s, y + 2 * s, 4.5f * s, 2 * s);

    // body: knee-long panjabi / kameez
    float bodyBottom = (L.type == 0 || L.type == 1) ? y + 16 * s : (L.type == 2 ? y + 20 * s * h : hip + 4 * s);
    if (L.type == 2 && !L.tupi) bodyBottom = hip - 2 * s;     // boy in shirt
    col(L.tr, L.tg, L.tb);
    glBegin(GL_QUADS);
    glVertex2f(x - 11 * s, bodyBottom); glVertex2f(x + 11 * s, bodyBottom);
    glVertex2f(x + 9 * s, shoulder); glVertex2f(x - 9 * s, shoulder); glEnd();
    col(L.tr * 0.85f, L.tg * 0.85f, L.tb * 0.85f);           // shaded side
    glBegin(GL_QUADS);
    glVertex2f(x + 4 * s, bodyBottom); glVertex2f(x + 11 * s, bodyBottom);
    glVertex2f(x + 9 * s, shoulder); glVertex2f(x + 4 * s, shoulder); glEnd();
    if (L.type == 0 || (L.type == 2 && L.tupi)) {            // placket and buttons
        col(L.tr * 0.75f, L.tg * 0.75f, L.tb * 0.75f);
        rect(x - 0.6f * s, shoulder - 14 * s, x + 0.6f * s, shoulder);
        col(1, 0.9f, 0.6f);
        for (int b = 0; b < 3; b++) circle(x, shoulder - 3 * s - b * 4 * s, 0.9f * s);
    }
    if (L.type == 1) {                                       // orna (scarf) across the chest
        col(L.br * 0.9f + 0.1f, L.bg * 0.9f, L.bb * 0.9f);
        glBegin(GL_QUADS);
        glVertex2f(x - 9 * s, shoulder - 2 * s); glVertex2f(x - 4 * s, shoulder);
        glVertex2f(x + 11 * s, shoulder - 20 * s); glVertex2f(x + 7 * s, shoulder - 23 * s); glEnd();
    }
    // arms
    bool longSleeve = !(L.type == 2 && !L.tupi);
    arm(x - 9 * s, shoulder - 2 * s, la, lb, s * h, L, longSleeve);
    arm(x + 9 * s, shoulder - 2 * s, ra, rb, s * h, L, longSleeve);
    // neck and head
    col(sk[0] * 0.9f, sk[1] * 0.9f, sk[2] * 0.9f);
    rect(x - 2.5f * s, shoulder, x + 2.5f * s, shoulder + 4 * s);
    float hy = shoulder + 11 * s;
    col(sk[0], sk[1], sk[2]);
    circle(x, hy, 8.5f * s);
    circle(x - 8.3f * s, hy, 1.8f * s); circle(x + 8.3f * s, hy, 1.8f * s);  // ears
    if (L.type == 1) {                                       // head covering
        col(L.br * 0.9f + 0.1f, L.bg * 0.9f, L.bb * 0.9f);
        halfCircle(x, hy + 1 * s, 10 * s);
        rect(x - 10 * s, hy - 8 * s, x - 7 * s, hy + 1 * s); rect(x + 7 * s, hy - 8 * s, x + 10 * s, hy + 1 * s);
    } else {
        col(0.10f, 0.07f, 0.06f);                            // hair
        halfCircle(x, hy + 2 * s, 8.8f * s);
        if (L.type == 3) { circle(x - 8 * s, hy - 2 * s, 3 * s); circle(x + 8 * s, hy - 2 * s, 3 * s); }
    }
    if (L.tupi) { col(0.98f, 0.98f, 0.96f); halfCircle(x, hy + 4 * s, 8.6f * s); col(0.85f, 0.85f, 0.82f); rect(x - 8.6f * s, hy + 4 * s, x + 8.6f * s, hy + 5 * s); }
    if (L.beard) { col(0.25f, 0.22f, 0.20f); glBegin(GL_TRIANGLE_FAN); glVertex2f(x, hy - 3 * s);
        for (int i = 0; i <= 20; i++) { float t = PI + PI * i / 20; glVertex2f(x + 8 * s * cos(t), hy - 1 * s + 11 * s * sin(t)); } glEnd(); }
    col(0.10f, 0.07f, 0.06f);                                // eyes
    circle(x - 3 * s, hy + 1 * s, 1.1f * s); circle(x + 3 * s, hy + 1 * s, 1.1f * s);
    if (!L.beard) { col(0.55f, 0.25f, 0.20f); glLineWidth(1.5f); arcLine(x, hy - 2 * s, 3 * s, 2 * s, PI * 1.15f, PI * 1.85f); glLineWidth(1); }
}


static void fairyLights(float x1, float y1, float x2, float y2, float sag, int n, int seed) {
    col(0.10f, 0.10f, 0.12f, 0.8f);
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i <= 30; i++) { float t = i / 30.0f; glVertex2f(mix(x1, x2, t), mix(y1, y2, t) - sag * 4 * t * (1 - t)); }
    glEnd();
    float cc[4][3] = {{1,0.35f,0.35f},{0.4f,1,0.5f},{0.4f,0.7f,1},{1,0.85f,0.3f}};
    for (int i = 0; i <= n; i++) {
        float t = (float)i / n, x = mix(x1, x2, t), y = mix(y1, y2, t) - sag * 4 * t * (1 - t) - 3;
        bool on = ((frameNo / 12) + i + seed) % 3 != 0;           // blinking
        float* c = cc[(i + seed) % 4];
        if (on) glow(x, y, 9, c[0], c[1], c[2], 0.5f);
        col(c[0], c[1], c[2], on ? 1 : 0.35f); circle(x, y, 2.6f);
    }
}

// =================== intro scene ===================
// building: x 300..800, y 60..600. 6 windows (2 columns x 3 floors)
static const float WX[2] = {335, 605}, WY[3] = {420, 250, 80};   // bottom-left corner of each window
static const float WW = 160, WH = 130;
static const char* CAPTIONS[6] = {
    "Khala receives a bowl of semai at her door...",
    "Apu and her cousin put on mehedi...",
    "Everyone enjoys the semai together...",
    "Dadu gives Eid salami!",
    "A gift reaches a poor home - the lamp is lit!",
    "Two friends hug each other: Eid Mubarak!"};

static void windowPos(int i, float& x, float& y) { x = WX[i % 2]; y = WY[i / 2]; }

static void smallGift(float x, float y, float s, float r, float g, float b) {
    col(r, g, b); rect(x - 8 * s, y, x + 8 * s, y + 12 * s);
    col(1.0f, 0.85f, 0.25f); rect(x - 1.2f * s, y, x + 1.2f * s, y + 12 * s); rect(x - 8 * s, y + 5 * s, x + 8 * s, y + 7 * s);
    triangle(x, y + 12 * s, x - 6 * s, y + 17 * s, x - 5 * s, y + 11 * s);
    triangle(x, y + 12 * s, x + 6 * s, y + 17 * s, x + 5 * s, y + 11 * s);
}

// a hand seen from above, with a mehedi pattern that grows (amount 0..1)
static void mehediHand(float x, float y, float s, float amount) {
    col(0.86f, 0.64f, 0.48f);
    ellipse(x, y, 13 * s, 16 * s);                                   // palm
    float fx[4] = {-9, -3, 3, 9}, fl[4] = {14, 18, 17, 13};
    for (int k = 0; k < 4; k++) { rect(x + fx[k] * s - 2.6f * s, y + 8 * s, x + fx[k] * s + 2.6f * s, y + (8 + fl[k]) * s);
                                  circle(x + fx[k] * s, y + (8 + fl[k]) * s, 2.6f * s); }
    thickLine(x - 12 * s, y - 2 * s, x - 22 * s, y + 8 * s, 5 * s);  // thumb
    rect(x - 9 * s, y - 26 * s, x + 9 * s, y - 12 * s);              // wrist
    col(0.55f, 0.20f, 0.08f);                                        // mehedi pattern appears dot by dot
    int dots = (int)(amount * 30);
    for (int k = 0; k < dots && k < 12; k++) { float a = k * PI / 6; circle(x + 7 * s * cos(a), y + 7 * s * sin(a), 1.1f * s); }
    if (dots > 12) circle(x, y, 3 * s);
    for (int k = 13; k < dots && k < 30; k++) { int fi = (k - 13) % 4; float fy = y + (10 + ((k - 13) / 4) * 4) * s;
                                                circle(x + fx[fi] * s, fy, 0.9f * s); }
}

// what happens inside each window (local coordinates 0..WW, 0..WH); lt = time since it opened
static void windowScene(int i, float lt) {
    float f = frameNo;
    // every room has its own wall colour and small decorations
    float wall[6][3] = {{1.0f,0.80f,0.55f},{0.98f,0.78f,0.80f},{0.85f,0.92f,0.75f},
                        {0.80f,0.86f,0.98f},{1.0f,0.80f,0.55f},{0.98f,0.88f,0.62f}};
    grad(0, 0, WW, WH, wall[i][0] * 0.92f, wall[i][1] * 0.85f, wall[i][2] * 0.80f, wall[i][0], wall[i][1], wall[i][2]);
    col(0.70f, 0.48f, 0.30f); rect(0, 0, WW, 10);                    // wooden floor
    col(0.62f, 0.40f, 0.24f); for (float x = 8; x < WW; x += 22) rect(x, 0, x + 1, 10);
    if (i != 4) {
        col(0.75f, 0.25f, 0.30f, 0.85f); rect(30, 1, 130, 7);         // rug
        col(1.0f, 0.80f, 0.30f, 0.85f); rect(30, 3, 130, 5);
        col(0.55f, 0.35f, 0.20f); rect(WW / 2 - 1, WH - 14, WW / 2 + 1, WH);   // hanging lamp
        glow(WW / 2, WH - 18, 26, 1, 0.9f, 0.5f, 0.5f);
        col(1.0f, 0.92f, 0.60f); halfCircle(WW / 2, WH - 20, 7);
        if (i == 0 || i == 3) { col(0.55f, 0.35f, 0.20f); rect(124, 86, 146, 108);     // framed picture
                                col(0.45f, 0.70f, 0.85f); rect(127, 89, 143, 105);
                                col(0.30f, 0.60f, 0.35f); triangle(127, 89, 143, 89, 135, 100); }
        if (i == 2) { col(1, 1, 1); circle(138, 104, 8); col(0.3f, 0.3f, 0.3f);          // wall clock
                      thickLine(138, 104, 138, 110, 1.5f); thickLine(138, 104, 143, 104, 1.5f); }
    }
    if (i == 0) {                                   // semai at the neighbour's door
        col(0.45f, 0.28f, 0.15f); rect(98, 10, 150, 104);               // door frame
        grad(103, 10, 145, 99, 0.95f, 0.85f, 0.65f, 0.90f, 0.75f, 0.55f);
        Look khala = {0.74f, 0.55f, 0.75f, 0.55f, 0.85f, 0.60f, 0.85f, 1, false, false};
        Look rafi  = {0.72f, 0.30f, 0.55f, 0.95f, 0.95f, 0.95f, 0.95f, 2, true, false};
        human(124, 10, 0.95f, khala, -70, 40, 8, -10);                 // taking the bowl
        human(55, 10, 1.0f, rafi, -10, 10, 70, 40);                    // giving the bowl
        col(0.96f, 0.96f, 0.96f); ellipse(90, 50, 13, 5); rect(77, 44, 103, 50);   // bowl of semai
        col(0.88f, 0.66f, 0.36f); ellipse(90, 52, 11, 4);
        col(1, 1, 1, 0.5f); glLineWidth(2); float st = fmod(f * 0.5f, 18);
        arcLine(88, 60 + st, 3, 6, -PI / 2, PI / 2); glLineWidth(1);
    } else if (i == 1) {                            // mehedi
        Look apu = {0.74f, 0.95f, 0.45f, 0.55f, 0.95f, 0.80f, 0.30f, 1, false, false};
        Look bon = {0.72f, 0.40f, 0.70f, 0.85f, 0.95f, 0.85f, 0.40f, 1, false, false};
        human(30, 22, 0.85f, apu, -10, 10, 80, 20);                    // holds out her hand
        human(130, 22, 0.85f, bon, -75, -25, 8, -10);                  // draws with the cone
        col(0.55f, 0.32f, 0.18f); rect(45, 10, 115, 14); rect(50, 14, 55, 36); rect(105, 14, 110, 36);
        col(0.62f, 0.38f, 0.22f); rect(42, 36, 118, 40);                // small table
        float amount = clamp01(lt / 150);
        mehediHand(78, 62, 1.15f, amount);                              // big hand on the table
        float cx = 92 + 6 * sin(f * 0.25f), cy = 70 + 5 * cos(f * 0.2f);
        col(0.45f, 0.75f, 0.35f); triangle(cx, cy, cx + 14, cy + 20, cx + 20, cy + 14);   // mehedi cone
        col(0.95f, 0.85f, 0.30f); circle(cx + 17, cy + 17, 3);
    } else if (i == 2) {                            // eating semai together
        Look abbu = {0.68f, 0.96f, 0.96f, 0.93f, 0.93f, 0.93f, 0.90f, 0, true, true};
        Look ammu = {0.76f, 0.92f, 0.35f, 0.55f, 0.95f, 0.70f, 0.30f, 1, false, false};
        Look kid  = {0.70f, 0.30f, 0.65f, 0.95f, 0.95f, 0.95f, 0.95f, 2, true, false};
        float eat = fmod(f, 60) < 30 ? 1 : 0;                           // spoon goes up to the mouth
        human(38, 30, 0.85f, abbu, -8, 10, eat ? 160 : 40, eat ? -120 : -30);
        human(80, 30, 0.85f, ammu, -8, 10, 30, -40);
        human(122, 30, 0.85f, kid, eat ? -160 : -40, eat ? 120 : 30, 8, -10);
        col(0.55f, 0.32f, 0.18f); rect(14, 10, 20, 52); rect(140, 10, 146, 52);
        col(0.98f, 0.97f, 0.92f); rect(10, 50, 150, 58);                // tablecloth
        col(0.85f, 0.25f, 0.30f); for (float x = 10; x < 150; x += 10) triangle(x, 50, x + 10, 50, x + 5, 43);
        for (int k = 0; k < 3; k++) { col(0.96f, 0.96f, 0.96f); ellipse(40 + k * 40, 62, 12, 5);
                                      col(0.88f, 0.66f, 0.36f); ellipse(40 + k * 40, 64, 9, 3.5f); }
    } else if (i == 3) {                            // Dadu gives salami
        Look dadu = {0.70f, 0.96f, 0.96f, 0.93f, 0.93f, 0.93f, 0.90f, 0, true, true};
        Look kid = {0.70f, 1.0f, 0.55f, 0.75f, 0.95f, 0.45f, 0.65f, 3, false, false};
        human(48, 10, 0.95f, dadu, -8, 10, 80, 10);
        float jump = fabs(sin(f * 0.15f)) * 6;
        human(112, 10, 0.95f, kid, -100, -20, -10, 10, jump);
        col(0.40f, 0.70f, 0.45f); rect(80, 60, 96, 68);
        col(0.25f, 0.50f, 0.30f); rect(82, 62, 94, 66);
        float up = fmod(f * 0.6f, 40);
        col(0.10f, 0.45f, 0.25f, 1 - up / 40); drawText(88, 80 + up, "+100 Tk", GLUT_BITMAP_HELVETICA_12);
    } else if (i == 4) {                            // a poor home: the gift lights the lamp
        float lit = clamp01((lt - 40) / 60);                            // room gets warm after the gift
        grad(0, 0, WW, WH, mix(0.30f, 0.95f, lit), mix(0.30f, 0.72f, lit), mix(0.40f, 0.45f, lit),
                           mix(0.38f, 0.90f, lit), mix(0.36f, 0.78f, lit), mix(0.48f, 0.55f, lit));
        col(0.45f, 0.38f, 0.28f, 0.18f);                                // bamboo wall pattern
        for (float y = 12; y < WH; y += 9) for (float x = ((int)(y / 9) % 2) * 6; x < WW; x += 12) rect(x, y, x + 6, y + 7);
        col(0.50f, 0.36f, 0.24f); rect(0, 0, WW, 10);
        col(0.82f, 0.70f, 0.42f); rect(20, 10, 140, 14);                // mat
        col(0.45f, 0.32f, 0.20f); rect(14, 74, 50, 78);                 // shelf
        col(0.40f, 0.42f, 0.45f); rect(26, 78, 36, 92);                 // kupi lamp
        if (lit > 0) { glow(31, 96, 90 * lit, 1, 0.75f, 0.30f, 0.45f * lit);
                       col(1, 0.75f, 0.25f, lit); triangle(27, 92, 35, 92, 31, 104); }
        Look ma  = {0.70f, mix(0.50f, 0.90f, lit), mix(0.47f, 0.50f, lit), mix(0.45f, 0.30f, lit),
                    0.42f, 0.40f, 0.40f, 1, false, false};
        Look kid0 = {0.66f, 0.52f, 0.50f, 0.48f, 0.40f, 0.38f, 0.36f, 2, false, false};
        Look kid1 = {0.66f, 0.25f, 0.72f, 0.58f, 0.96f, 0.96f, 0.96f, 2, true, false};
        human(70, 14, 0.9f, ma, -10, 10, lit > 0.5f ? 40 : 12, lit > 0.5f ? -70 : -20);
        if (lit > 0.5f) {
            human(115, 14, 0.9f, kid1, -150, 0, 150, 0, fabs(sin(f * 0.15f)) * 6);   // happy, new clothes
            smallGift(140, 14, 0.8f, 0.90f, 0.20f, 0.30f);
        } else human(115, 14, 0.9f, kid0, -20, 60, 20, -60);
    } else {                                        // two friends hug
        Look a = {0.72f, 0.30f, 0.55f, 0.95f, 0.95f, 0.95f, 0.95f, 2, true, false};
        Look b = {0.66f, 0.95f, 0.75f, 0.30f, 0.96f, 0.96f, 0.96f, 2, true, false};
        float sway = 2 * sin(f * 0.1f);
        human(66 + sway, 10, 1.05f, a, -8, 10, 95, 0);
        human(100 + sway, 10, 1.05f, b, -95, 0, 8, -10);
        col(1, 1, 1, 0.95f); roundRect(40, 100, 126, 120, 8);
        triangle(80, 101, 90, 101, 84, 92);
        col(0.20f, 0.12f, 0.30f); strokeText(83, 106, "Eid Mubarak!", 0.075f, 1.5f);   // stroke text scales with the zoom
        for (int k = 0; k < 4; k++) { float a2 = f * 0.05f + k * 1.57f;
            col(1, 0.4f, 0.5f, 0.8f); circle(83 + 50 * cos(a2), 70 + 30 * sin(a2), 2.5f); }   // little hearts of joy
    }
    // curtains on both sides
    col(0.75f, 0.20f, 0.30f);
    glBegin(GL_QUADS); glVertex2f(0, 0); glVertex2f(12, 0); glVertex2f(22, WH); glVertex2f(0, WH); glEnd();
    glBegin(GL_QUADS); glVertex2f(WW - 12, 0); glVertex2f(WW, 0); glVertex2f(WW, WH); glVertex2f(WW - 22, WH); glEnd();
}

// wooden shutters: open = 0 (closed) .. 1 (fully open). Each half is squeezed
// towards its hinge with a scaling transformation, so it looks like it swings open.
static void shutters(float x, float y, float open) {
    // a little bounce at the end of the opening (ease-out-back)
    float o = open < 1 ? open : 1;
    float e = 1 + 2.2f * pow(o - 1, 3) + 1.2f * pow(o - 1, 2);
    if (open <= 0) e = 0;
    float shade = 1 - 0.35f * o;                         // the shutter turns away from the light
    for (int side = 0; side < 2; side++) {
        float sx = 1 - 0.85f * e;                        // width shrinks as it opens
        glPushMatrix();
        glTranslatef(side ? x + WW : x, y, 0);           // hinge on the outer edge
        glScalef(side ? -sx : sx, 1, 1);                 // right shutter is a mirror image
        col(0.45f * shade, 0.26f * shade, 0.14f * shade); rect(0, 0, WW / 2, WH);
        col(0.55f * shade, 0.33f * shade, 0.18f * shade);
        rect(6, 8, WW / 2 - 6, WH / 2 - 4); rect(6, WH / 2 + 4, WW / 2 - 6, WH - 8);
        col(0.38f * shade, 0.22f * shade, 0.12f * shade); rect(WW / 2 - 3, 0, WW / 2, WH);
        col(0.30f * shade, 0.17f * shade, 0.09f * shade);           // louvres of the shutter
        for (float ly = 14; ly < WH / 2 - 8; ly += 7) rect(10, ly, WW / 2 - 10, ly + 2);
        col(0.85f, 0.70f, 0.30f); circle(WW / 2 - 9, WH / 2, 2.5f);   // handle
        glPopMatrix();
    }
}

// people walking in the street (seen after the zoom out)
static void streetLife(float f) {
    for (int k = 0; k < 2; k++) {                        // street lamps
        float lx = k ? 880 : 220;
        col(0.15f, 0.15f, 0.18f); rect(lx - 2, 10, lx + 2, 120); rect(lx - 2, 116, lx + 14, 120);
        glow(lx + 12, 112, 50, 1, 0.85f, 0.5f, 0.40f); col(1, 0.92f, 0.65f); circle(lx + 12, 112, 4);
    }
    Look ab = {0.68f, 0.96f, 0.96f, 0.93f, 0.93f, 0.93f, 0.90f, 0, true, true};
    Look am = {0.76f, 0.92f, 0.35f, 0.55f, 0.95f, 0.70f, 0.30f, 1, false, false};
    Look kd = {0.70f, 0.30f, 0.55f, 0.95f, 0.95f, 0.95f, 0.95f, 2, true, false};
    float x = fmod(f * 0.8f, 1300) - 100, sw = 18 * sin(f * 0.2f);
    human(x, 8, 0.55f, ab, -sw, 10, sw, -10);
    human(x + 20, 8, 0.55f, am, sw, 10, -sw, -10);
    human(x + 40, 8, 0.55f, kd, -sw, 10, 150, 0);
    float tx = x + 46, ty = 48;                          // sparkler in the child's hand
    glow(tx, ty, 14, 1, 0.85f, 0.4f, 0.6f);
    glBegin(GL_LINES);
    for (int k = 0; k < 10; k++) { float a = rnd(k * 5 + (int)f) * PI;
        glColor4f(1, 0.95f, 0.6f, 1); glVertex2f(tx, ty); glColor4f(1, 0.6f, 0.2f, 0); glVertex2f(tx + 10 * cos(a), ty + 10 * sin(a)); }
    glEnd();
    float x2 = 1150 - fmod(f * 0.6f + 500, 1300);        // two friends walking the other way
    Look b1 = {0.66f, 0.95f, 0.75f, 0.30f, 0.96f, 0.96f, 0.96f, 2, true, false};
    human(x2, 8, 0.5f, b1, -sw, 10, sw, -10);
    human(x2 + 18, 8, 0.5f, kd, sw, 10, -sw, -10);
}

// a firework burst for the ending
static void burst(float cx, float cy, float p, float r, float g, float b) {
    if (p <= 0 || p >= 1) return;
    float rad = 70 * sqrt(p), fall = 25 * p * p, a = 1 - p;
    glow(cx, cy - fall, rad * 1.2f, r, g, b, 0.22f * a);
    glLineWidth(2); glBegin(GL_LINES);
    for (int i = 0; i < 24; i++) { float ang = i * 2 * PI / 24;
        glColor4f(r, g, b, 0); glVertex2f(cx + rad * 0.5f * cos(ang), cy + rad * 0.5f * sin(ang) - fall * 0.5f);
        glColor4f(r, g, b, a); glVertex2f(cx + rad * cos(ang), cy + rad * sin(ang) - fall); }
    glEnd(); glLineWidth(1);
}

// darker neighbouring buildings with small lit windows (seen after the zoom out)
static void neighbours() {
    float b[4][3] = {{40, 160, 430}, {165, 285, 520}, {815, 945, 470}, {950, 1080, 380}};
    for (int k = 0; k < 4; k++) {
        col(0.22f, 0.18f, 0.30f); rect(b[k][0], 52, b[k][1], b[k][2]);
        col(0.18f, 0.15f, 0.25f); rect(b[k][0] - 4, b[k][2], b[k][1] + 4, b[k][2] + 8);
        for (float y = 90; y < b[k][2] - 30; y += 55)
            for (float x = b[k][0] + 14; x < b[k][1] - 20; x += 36) {
                bool lit = ((int)(x + y + k * 7) / 9) % 3 != 0;
                if (lit) { glow(x + 8, y + 12, 22, 1, 0.75f, 0.35f, 0.2f); col(1.0f, 0.80f, 0.45f); }
                else col(0.12f, 0.10f, 0.18f);
                rect(x, y, x + 16, y + 24);
            }
    }
    fairyLights(165, 528, 285, 528, 8, 8, 2);
    fairyLights(815, 478, 945, 478, 8, 9, 1);
}

static void drawBuilding(float openAmount[6], bool allLit, float f, float PER) {
    neighbours();
    // brick wall
    col(0.78f, 0.45f, 0.36f); rect(300, 60, 800, 600);
    col(0.70f, 0.39f, 0.31f);
    for (int r = 0; r < 54; r++) {                       // brick rows
        float y = 60 + r * 10;
        rect(300, y, 800, y + 1);
        for (float x = 300 + (r % 2) * 14; x < 800; x += 28) rect(x, y, x + 1, y + 10);
    }
    hgrad(740, 60, 800, 600, 0, 0, 0, 0, 0, 0, 0, 0.25f);
    col(0.60f, 0.34f, 0.27f); rect(290, 600, 810, 616);  // roof edge
    fairyLights(290, 618, 810, 618, -10, 30, 0);
    col(0.35f, 0.22f, 0.16f); roundRect(480, 572, 620, 596, 6);         // name of the building
    col(1.0f, 0.85f, 0.45f); strokeText(550, 578, "SHANTI NEER", 0.11f, 1.5f);
    col(0.60f, 0.34f, 0.27f); rect(290, 52, 810, 62);    // base
    for (int i = 0; i < 6; i++) {
        float x, y; windowPos(i, x, y);
        if (allLit || openAmount[i] > 0) glow(x + WW / 2, y + WH / 2, 120, 1, 0.75f, 0.35f, 0.25f * (allLit ? 1 : openAmount[i]));
        col(0.88f, 0.80f, 0.68f); archShape(x - 10, y - 10, x + WW + 10, y + WH + 40);   // stone arch frame
        col(0.20f, 0.12f, 0.10f); rect(x - 4, y - 4, x + WW + 4, y + WH + 4);
        if (openAmount[i] > 0 || allLit) {
            glPushMatrix(); glTranslatef(x, y, 0); windowScene(i, f - i * PER - 15); glPopMatrix();
        } else {
            grad(x, y, x + WW, y + WH, 0.20f, 0.15f, 0.25f, 0.30f, 0.22f, 0.32f);
        }
        shutters(x, y, allLit ? 1 : openAmount[i]);
        float lit = allLit ? 1 : openAmount[i];
        if (lit > 0) {                                   // warm light falls on the wall below
            glBegin(GL_QUADS);
            glColor4f(1, 0.80f, 0.45f, 0.30f * lit); glVertex2f(x, y - 4); glVertex2f(x + WW, y - 4);
            glColor4f(1, 0.80f, 0.45f, 0.0f);        glVertex2f(x + WW + 25, y - 60); glVertex2f(x - 25, y - 60);
            glEnd();
        }
        col(0.80f, 0.70f, 0.55f); rect(x - 14, y - 12, x + WW + 14, y - 4);    // window sill
        for (int p = 0; p < 2; p++) {                    // flower pots on the sill
            float px = p ? x + WW - 4 : x + 4;
            col(0.70f, 0.35f, 0.22f); rect(px - 7, y - 4, px + 7, y + 6);
            col(0.25f, 0.55f, 0.28f); circle(px - 4, y + 10, 5); circle(px + 4, y + 11, 5); circle(px, y + 14, 5);
            float fc[2][3] = {{0.95f, 0.35f, 0.45f}, {1.0f, 0.85f, 0.30f}};
            col(fc[(i + p) % 2][0], fc[(i + p) % 2][1], fc[(i + p) % 2][2]);
            circle(px - 4, y + 14, 2.2f); circle(px + 3, y + 17, 2.2f);
        }
    }
}

void drawSharing() {
    float f = slideFrame;
    const float PER = 75;                                // frames spent at each window
    float zoomOutStart = 6 * PER, zoomOutEnd = zoomOutStart + 110;

    // night sky behind everything
    grad(0, 0, WIN_W, WIN_H, 0.25f, 0.20f, 0.45f, 0.05f, 0.06f, 0.18f);
    for (int i = 0; i < 50; i++) {
        float sx = fabs(rnd(i + 3)) * WIN_W, sy = 300 + fabs(rnd(i + 60)) * 500;
        col(1, 1, 0.9f, 0.3f + 0.6f * fabs(sin(f * 0.05f + i))); circle(sx, sy, i % 6 == 0 ? 1.8f : 1.1f);
    }

    // which window the camera looks at, and how open each window is
    float openAmount[6];
    for (int i = 0; i < 6; i++) openAmount[i] = clamp01((f - i * PER - 15) / 35);
    int cur = (int)(f / PER); if (cur > 5) cur = 5;
    float t = clamp01((f - cur * PER) / 20);             // quick pan to the new window
    float ax, ay, bx, by;
    windowPos(cur > 0 ? cur - 1 : 0, ax, ay); windowPos(cur, bx, by);
    float fx = mix(ax, bx, t * t * (3 - 2 * t)) + WW / 2, fy = mix(ay, by, t * t * (3 - 2 * t)) + WH / 2;
    float z = 3.2f - 0.6f * sin(PI * t) * (cur > 0 ? 1 : 0);   // camera pulls back a little while moving
    if (f < 30) z = mix(2.0f, 3.2f, f / 30 * f / 30);         // at the start the camera moves in
    float zo = clamp01((f - zoomOutStart) / (zoomOutEnd - zoomOutStart));
    zo = zo * zo * (3 - 2 * zo);
    z = mix(z, 1.0f, zo);                                // camera zooms out (scaling)
    fx = mix(fx, 550, zo); fy = mix(fy, 400, zo);

    // moon rises after the zoom out
    float moonUp = clamp01((f - zoomOutEnd) / 90);
    glow(930, mix(560, 690, moonUp), 110, 1, 0.9f, 0.6f, 0.35f * moonUp);
    moon(930, mix(560, 690, moonUp), 34, moonUp);

    glPushMatrix();
    glTranslatef(550, 400, 0);
    glScalef(z, z, 1);
    glTranslatef(-fx, -fy, 0);
    grad(-500, 0, 1600, 52, 0.18f, 0.16f, 0.22f, 0.28f, 0.25f, 0.30f);   // street
    drawBuilding(openAmount, zo >= 1, f, PER);
    streetLife(f);
    glPopMatrix();

    // caption for the window being shown
    if (zo < 0.3f) {
        const char* cap = CAPTIONS[cur];
        float a = clamp01((f - cur * PER - 20) / 15) * (1 - zo / 0.3f);
        float cw = textW(cap, GLUT_BITMAP_HELVETICA_18);
        col(0.06f, 0.05f, 0.12f, 0.8f * a); roundRect(550 - cw / 2 - 22, 40, 550 + cw / 2 + 22, 74, 17);
        col(1.0f, 0.92f, 0.70f, a); drawTextCentered(550, 51, cap, GLUT_BITMAP_HELVETICA_18);
    }

    // message after the zoom out
    float ta = clamp01((f - zoomOutEnd - 30) / 60);
    if (ta > 0) {
        float q = f - zoomOutEnd - 30;                   // fireworks celebrate the ending
        burst(160, 620, fmod(q, 140) / 140, 1.0f, 0.4f, 0.5f);
        burst(950, 560, fmod(q + 60, 140) / 140, 0.4f, 0.8f, 1.0f);
        burst(240, 470, fmod(q + 100, 160) / 160, 1.0f, 0.85f, 0.3f);
        col(0.08f, 0.05f, 0.15f, 0.55f * ta); roundRect(250, 640, 850, 790, 24);
        float ts = 0.40f * (0.85f + 0.15f * ta);         // title grows into place (scaling)
        col(0.30f, 0.12f, 0.20f, 0.6f * ta); strokeText(553, 717, "Eid Mubarak!", ts, 6);
        col(1.0f, 0.84f, 0.35f, ta); strokeText(550, 720, "Eid Mubarak!", ts, 5);
        col(0.95f, 0.92f, 1.0f, ta); drawTextCentered(550, 685, "Eid is complete when joy is shared", GLUT_BITMAP_TIMES_ROMAN_24);
        col(1.0f, 0.85f, 0.45f, ta); drawTextCentered(550, 656, "- EidVille -", GLUT_BITMAP_HELVETICA_18);
        float blink = 0.5f + 0.5f * sin(f * 0.1f);
        col(0.06f, 0.05f, 0.12f, 0.8f * ta); roundRect(420, 28, 680, 58, 15);
        col(1, 1, 1, ta * blink); drawTextCentered(550, 38, "Press SPACE to watch again", GLUT_BITMAP_HELVETICA_18);
    }
}