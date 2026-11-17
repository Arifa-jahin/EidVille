// ============================================================
//  moonnight.cpp  -  Slide 1: Chand Raat (the Eid moon is seen)
//  Owner: Arifa
// ============================================================
#include "common.h"

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

// =========================================================
//  SLIDE 1 : CHAND RAAT
// =========================================================
static void cloud(float x, float y, float s, float k) {
    float c[7][3] = {{0,0,22},{24,6,26},{50,0,20},{-24,-2,18},{12,-8,20},{38,-8,18},{-6,8,18}};
    col(0.55f * k + 0.2f, 0.36f * k + 0.12f, 0.52f * k + 0.15f);         // shaded top
    for (int i = 0; i < 7; i++) ellipse(x + c[i][0] * s, y + c[i][1] * s + 4 * s, c[i][2] * s * 1.3f, c[i][2] * s * 0.8f);
    col(1.0f, 0.62f, 0.52f, 0.9f);                                        // lit by the sunset below
    for (int i = 0; i < 7; i++) ellipse(x + c[i][0] * s, y + c[i][1] * s - 3 * s, c[i][2] * s * 1.2f, c[i][2] * s * 0.55f);
}
static void dayCloud(float x, float y, float s) {
    float c[6][3] = {{0,0,22},{24,6,26},{50,0,20},{-24,-2,18},{12,10,20},{36,-6,18}};
    col(0.86f, 0.90f, 0.96f); for (int i = 0; i < 6; i++) ellipse(x + c[i][0] * s, y + c[i][1] * s - 3 * s, c[i][2] * s * 1.3f, c[i][2] * s * 0.8f);
    col(1, 1, 1);             for (int i = 0; i < 6; i++) ellipse(x + c[i][0] * s, y + c[i][1] * s + 2 * s, c[i][2] * s * 1.2f, c[i][2] * s * 0.7f);
}
static void bird(float x, float y, float s, float flap) {
    col(0.15f, 0.10f, 0.20f); glLineWidth(2);
    glBegin(GL_LINE_STRIP);
    glVertex2f(x - 9 * s, y + flap * s); glVertex2f(x - 4 * s, y + 2 * s); glVertex2f(x, y);
    glVertex2f(x + 4 * s, y + 2 * s); glVertex2f(x + 9 * s, y + flap * s);
    glEnd(); glLineWidth(1);
}
static void bunting(float x1, float y1, float x2, float y2, float sag, float phase, int n) {
    float c6[6][3] = {{0.96f,0.28f,0.36f},{1.0f,0.76f,0.18f},{0.25f,0.78f,0.48f},
                      {0.28f,0.58f,0.98f},{0.78f,0.38f,0.92f},{1.0f,0.52f,0.18f}};
    float sway = 5 * sin(frameNo * 0.05f + phase);
    col(0.12f, 0.10f, 0.14f);
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i <= 40; i++) {
        float t = i / 40.0f, d = 4 * t * (1 - t);
        glVertex2f(mix(x1, x2, t), mix(y1, y2, t) - (sag - sway) * d);
    }
    glEnd();
    for (int i = 1; i < n; i++) {
        float t = (float)i / n, t2 = t + 0.62f / n;
        float d1 = 4 * t * (1 - t), d2 = 4 * t2 * (1 - t2);
        float ax = mix(x1, x2, t),  ay = mix(y1, y2, t)  - (sag - sway) * d1;
        float bx = mix(x1, x2, t2), by = mix(y1, y2, t2) - (sag - sway) * d2;
        float* c = c6[(i + (int)phase) % 6];
        col(c[0], c[1], c[2]);
        float flut = 3 * sin(frameNo * 0.12f + i);
        triangle(ax, ay, bx, by, (ax + bx) / 2 + flut, (ay + by) / 2 - 17);
        col(c[0] * 0.75f, c[1] * 0.75f, c[2] * 0.75f);
        triangle((ax + bx) / 2, (ay + by) / 2, bx, by, (ax + bx) / 2 + flut, (ay + by) / 2 - 17);
    }
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
static void window(float x, float y, float w, float h, bool lit, bool grill) {
    col(0.30f, 0.26f, 0.26f); rect(x - 3, y - 3, x + w + 3, y + h + 3);         // frame
    if (lit) { glow(x + w / 2, y + h / 2, w * 1.3f, 1, 0.75f, 0.35f, 0.22f);
               grad(x, y, x + w, y + h, 1.0f, 0.70f, 0.35f, 1.0f, 0.88f, 0.58f); }
    else grad(x, y, x + w, y + h, 0.16f, 0.15f, 0.24f, 0.26f, 0.24f, 0.34f);
    if (grill) { col(0.20f, 0.18f, 0.18f); for (float gx = x + w / 4; gx < x + w; gx += w / 4) rect(gx - 0.8f, y, gx + 0.8f, y + h); rect(x, y + h / 2 - 0.8f, x + w, y + h / 2 + 0.8f); }
    col(0.42f, 0.36f, 0.34f); rect(x - 6, y + h + 3, x + w + 6, y + h + 8);   // sunshade (chajja)
}
static void rickshaw(float x, float y) {      // facing left
    float spin = -frameNo * 0.05f;
    for (int w = 0; w < 2; w++) {
        float cx = w ? x + 60 : x - 30, r = w ? 26 : 20;
        col(0.08f, 0.08f, 0.10f); glLineWidth(4); arcLine(cx, y + r, r, r, 0, 2 * PI); glLineWidth(1);
        col(0.75f, 0.75f, 0.78f); glBegin(GL_LINES);
        for (int k = 0; k < 6; k++) { float a = spin + k * PI / 6; glVertex2f(cx + r * cos(a), y + r + r * sin(a)); glVertex2f(cx - r * cos(a), y + r - r * sin(a)); }
        glEnd(); col(0.85f, 0.2f, 0.25f); circle(cx, y + r, 4);
    }
    col(0.20f, 0.20f, 0.24f); thickLine(x - 30, y + 20, x + 30, y + 40, 4); thickLine(x - 30, y + 20, x - 22, y + 70, 4);
    col(0.15f, 0.15f, 0.18f); rect(x - 32, y + 70, x - 12, y + 73);              // handle
    col(0.85f, 0.25f, 0.45f); rect(x + 20, y + 30, x + 100, y + 70);              // decorated body
    col(1.0f, 0.80f, 0.25f); rect(x + 20, y + 30, x + 100, y + 34); rect(x + 20, y + 66, x + 100, y + 70);
    for (int i = 0; i < 4; i++) { col(0.25f, 0.70f, 0.85f); circle(x + 32 + i * 19, y + 50, 6); col(1, 1, 1); circle(x + 32 + i * 19, y + 50, 2.5f); }
    col(0.30f, 0.15f, 0.40f); rect(x + 22, y + 70, x + 98, y + 82);              // seat
    // folded hood: a curved shell made of a triangle strip between two ellipse arcs
    float hx = x + 60, hy = y + 82;
    col(0.12f, 0.10f, 0.14f);
    glBegin(GL_TRIANGLE_STRIP);
    for (int i = 0; i <= 24; i++) {
        float t = (125.0f * i / 24) * PI / 180;
        glVertex2f(hx + 46 * cos(t), hy + 100 * sin(t));
        glVertex2f(hx + 36 * cos(t), hy + 90 * sin(t));
    }
    glEnd();
    col(0.45f, 0.10f, 0.22f);                                                    // inside of the hood
    glBegin(GL_TRIANGLE_FAN); glVertex2f(hx + 10, hy);
    for (int i = 0; i <= 24; i++) { float t = (125.0f * i / 24) * PI / 180; glVertex2f(hx + 36 * cos(t), hy + 90 * sin(t)); }
    glEnd();
    col(0.25f, 0.22f, 0.28f);                                                    // folds of the hood
    for (int k = 1; k < 4; k++) { float t = (30.0f * k) * PI / 180;
        thickLine(hx + 36 * cos(t), hy + 90 * sin(t), hx + 46 * cos(t), hy + 100 * sin(t), 2); }
    col(1.0f, 0.75f, 0.2f); glLineWidth(3); arcLine(hx, hy, 46, 100, 0, 125 * PI / 180); glLineWidth(1);
    for (int i = 0; i < 5; i++) { float t = (70 + i * 12) * PI / 180; float tx = hx + 46 * cos(t), ty = hy + 100 * sin(t);
        col(0.95f, 0.3f, 0.3f); thickLine(tx, ty, tx, ty - 10, 2); circle(tx, ty - 11, 2.2f); }   // tassels
}
// sky lantern (fanush) rising slowly
static void fanush(float x, float y, float s) {
    float fl = 0.8f + 0.2f * sin(frameNo * 0.3f + x);
    glow(x, y, 34 * s, 1, 0.6f, 0.2f, 0.35f * fl);
    glBegin(GL_QUADS);
    glColor4f(1.0f, 0.55f, 0.20f, 1); glVertex2f(x - 7 * s, y - 10 * s); glVertex2f(x + 7 * s, y - 10 * s);
    glColor4f(1.0f, 0.85f, 0.45f, 1); glVertex2f(x + 10 * s, y + 10 * s); glVertex2f(x - 10 * s, y + 10 * s);
    glEnd();
    col(1, 1, 0.8f, fl); ellipse(x, y - 8 * s, 2.5f * s, 3.5f * s);
}
// firework: rocket goes up, then bursts into falling sparks (start = frame offset)
static void firework(float cx, float cy, int start, float r, float g, float b) {
    float p = fmod(frameNo + start, 170) / 170.0f;
    if (p < 0.25f) {                                          // rocket rising
        float y = mix(200, cy, p / 0.25f);
        col(1, 0.9f, 0.6f); circle(cx, y, 2.5f);
        col(1, 0.7f, 0.3f, 0.5f); rect(cx - 1, y - 25, cx + 1, y);
        return;
    }
    float q = (p - 0.25f) / 0.75f, rad = 90 * sqrt(q), fall = 40 * q * q, a = 1 - q;
    glow(cx, cy - fall, rad * 1.2f, r, g, b, 0.25f * a);
    glLineWidth(2); glBegin(GL_LINES);                        // streaks
    for (int i = 0; i < 28; i++) {
        float ang = i * 2 * PI / 28;
        glColor4f(r, g, b, 0); glVertex2f(cx + rad * 0.55f * cos(ang), cy + rad * 0.55f * sin(ang) - fall * 0.5f);
        glColor4f(r, g, b, a); glVertex2f(cx + rad * cos(ang), cy + rad * sin(ang) - fall);
    }
    glEnd(); glLineWidth(1);
    glPointSize(4); glBegin(GL_POINTS);                       // bright tips
    for (int i = 0; i < 28; i++) {
        float ang = i * 2 * PI / 28;
        glColor4f(1, 1, 0.85f, a); glVertex2f(cx + rad * cos(ang), cy + rad * sin(ang) - fall);
    }
    glEnd(); glPointSize(1);
}
// balloon seller with a big bunch of balloons
static void balloonSeller(float x, float y) {
    Look sl = {0.62f, 0.85f, 0.85f, 0.80f, 0.30f, 0.45f, 0.35f, 0, false, true};
    float bc[6][3] = {{0.95f,0.25f,0.30f},{0.30f,0.60f,0.95f},{1.0f,0.80f,0.25f},{0.40f,0.85f,0.45f},{0.85f,0.40f,0.90f},{1.0f,0.55f,0.20f}};
    float hx = x + 22, hy = y + 62;
    col(0.45f, 0.30f, 0.15f); thickLine(hx, hy - 10, hx + 2, hy + 55, 3);
    for (int i = 0; i < 11; i++) {
        float bx = hx + 2 + 30 * cos(i * 0.9f) * (i % 3 ? 1 : 0.5f) + 3 * sin(frameNo * 0.04f + i);
        float by = hy + 82 + 22 * sin(i * 1.3f) + (i % 4) * 5;
        col(0.3f, 0.3f, 0.3f, 0.7f); glBegin(GL_LINES); glVertex2f(hx + 2, hy + 55); glVertex2f(bx, by - 13); glEnd();
        float* c = bc[i % 6];
        col(c[0], c[1], c[2]); ellipse(bx, by, 11, 14);
        col(1, 1, 1, 0.45f); ellipse(bx - 4, by + 5, 3, 4);
    }
    human(x, y, 1.3f, sl, -10, 20, 40, 100);
}
// tea stall with kettle and steam
static void teaStall(float x, float y) {
    col(0.45f, 0.28f, 0.16f); rect(x, y, x + 90, y + 50);
    col(0.38f, 0.22f, 0.12f); rect(x + 60, y, x + 90, y + 50);
    col(0.85f, 0.30f, 0.25f); rect(x - 6, y + 50, x + 96, y + 56);
    col(0.30f, 0.30f, 0.32f); rect(x + 5, y + 56, x + 8, y + 110); rect(x + 82, y + 56, x + 85, y + 110);
    col(0.90f, 0.35f, 0.25f); glBegin(GL_QUADS); glVertex2f(x - 12, y + 108); glVertex2f(x + 102, y + 108); glVertex2f(x + 92, y + 124); glVertex2f(x - 2, y + 124); glEnd();
    col(1, 0.95f, 0.85f); for (float sx = x - 8; sx < x + 98; sx += 20) rect(sx, y + 108, sx + 10, y + 124);
    col(0.65f, 0.65f, 0.68f); ellipse(x + 30, y + 66, 13, 10); rect(x + 41, y + 64, x + 52, y + 68); rect(x + 26, y + 74, x + 34, y + 78);
    col(0.95f, 0.95f, 0.92f); for (int i = 0; i < 3; i++) rect(x + 58 + i * 9, y + 56, x + 64 + i * 9, y + 64);
    col(0.9f, 0.9f, 0.9f, 0.45f); glLineWidth(2);
    float st = fmod(frameNo * 0.6f, 30);
    arcLine(x + 50, y + 74 + st, 3, 7, -PI / 2, PI / 2); arcLine(x + 54, y + 88 + st, 3, 7, PI / 2, PI * 1.5f); glLineWidth(1);
    col(1, 1, 1); drawTextCentered(x + 45, y + 30, "CHA", GLUT_BITMAP_HELVETICA_12);
}
static void sparkler(float x, float y, int seed) {
    col(0.35f, 0.32f, 0.30f); thickLine(x, y, x + 6, y + 24, 1.5f);
    float tx = x + 6, ty = y + 24;
    glow(tx, ty, 26, 1, 0.85f, 0.4f, 0.55f);
    glLineWidth(1.5f); glBegin(GL_LINES);
    for (int i = 0; i < 18; i++) {
        float a = rnd(i * 7 + frameNo + seed) * PI, len = 6 + fabs(rnd(i + frameNo * 3 + seed)) * 16;
        glColor4f(1, 0.95f, 0.6f, 1); glVertex2f(tx, ty);
        glColor4f(1, 0.6f, 0.2f, 0); glVertex2f(tx + len * cos(a), ty + len * sin(a));
    }
    glEnd(); glLineWidth(1);
}
static void bicycleRider(float bx, float wy) {
    float ang = -frameNo * 0.18f, wr = 19;
    Look boy = {0.62f, 0.38f, 0.72f, 0.90f, 0.32f, 0.20f, 0.38f, 2, false, false};
    for (int w = 0; w < 2; w++) {
        float cx = bx + (w == 0 ? -30 : 30);
        col(0.06f, 0.06f, 0.08f); glLineWidth(4); arcLine(cx, wy, wr, wr, 0, 2 * PI); glLineWidth(1);
        col(0.78f, 0.78f, 0.82f); glBegin(GL_LINES);
        for (int k = 0; k < 6; k++) { float a = ang + k * PI / 6; glVertex2f(cx + wr * cos(a), wy + wr * sin(a)); glVertex2f(cx - wr * cos(a), wy - wr * sin(a)); }
        glEnd();
    }
    col(0.82f, 0.16f, 0.18f);
    thickLine(bx - 30, wy, bx - 2, wy, 3.5f); thickLine(bx - 2, wy, bx - 10, wy + 30, 3.5f);
    thickLine(bx - 30, wy, bx - 10, wy + 30, 3.5f); thickLine(bx - 10, wy + 30, bx + 20, wy + 30, 3.5f);
    thickLine(bx - 2, wy, bx + 20, wy + 30, 3.5f); thickLine(bx + 20, wy + 30, bx + 30, wy, 3.5f);
    thickLine(bx + 20, wy + 30, bx + 22, wy + 40, 3.5f);
    col(0.1f, 0.1f, 0.1f); rect(bx - 18, wy + 30, bx - 3, wy + 35); rect(bx + 16, wy + 39, bx + 30, wy + 42);
    float pa = ang * 1.2f, s = 1.25f;
    float p1x = bx - 2 + 9 * cos(pa), p1y = wy + 9 * sin(pa);
    col(boy.br, boy.bg, boy.bb);                                    // legs push the pedals
    thickLine(bx - 10, wy + 36, p1x, p1y, 7);
    thickLine(bx - 10, wy + 36, bx - 2 - 9 * cos(pa), wy - 9 * sin(pa), 7);
    col(boy.tr, boy.tg, boy.tb);
    glBegin(GL_QUADS); glVertex2f(bx - 16, wy + 34); glVertex2f(bx - 2, wy + 34); glVertex2f(bx + 6, wy + 66); glVertex2f(bx - 8, wy + 66); glEnd();
    arm(bx + 1, wy + 62, 70, 30, s, boy, false);                    // arm to the handle
    float hy = wy + 78;
    col(boy.skin, boy.skin * 0.72f, boy.skin * 0.55f); circle(bx + 2, hy, 10);
    col(0.08f, 0.06f, 0.05f); halfCircle(bx - 1, hy + 2, 10); circle(bx - 6, hy + 6, 5);
    col(0.08f, 0.06f, 0.05f); circle(bx + 6, hy + 3, 1.6f);         // eye looking up at the moon
}

void drawMoonNight() {
    float f = slideFrame;                 // time since this slide appeared
    float moonA = clamp01((f - 60) / 120);
    float titleA = clamp01((f - 200) / 80);
    bool  seen = f > 180;                 // the moon has been seen: people celebrate
    float d = clamp01(f / 260);           // the evening slowly gets darker

    // ---- sky: three colour bands ----
    grad(0, 200, WIN_W, 430, 1.0f, mix(0.70f, 0.62f, d), mix(0.42f, 0.36f, d),
         mix(0.96f, 0.92f, d), mix(0.60f, 0.50f, d), mix(0.50f, 0.48f, d));
    grad(0, 430, WIN_W, 620, mix(0.96f, 0.92f, d), mix(0.60f, 0.50f, d), mix(0.50f, 0.48f, d),
         mix(0.62f, 0.42f, d), mix(0.42f, 0.28f, d), mix(0.58f, 0.50f, d));
    grad(0, 620, WIN_W, WIN_H, mix(0.62f, 0.42f, d), mix(0.42f, 0.28f, d), mix(0.58f, 0.50f, d),
         mix(0.30f, 0.12f, d), mix(0.24f, 0.10f, d), mix(0.48f, 0.28f, d));
    for (int i = 0; i < 40; i++) {                                    // twinkling stars
        float sx = fabs(rnd(i)) * WIN_W, sy = 600 + fabs(rnd(i + 100)) * 200;
        float tw = 0.4f + 0.6f * fabs(sin(f * 0.06f + i));
        col(1, 1, 0.92f, tw * clamp01((sy - 600) / 120)); circle(sx, sy, i % 5 == 0 ? 2.0f : 1.2f);
    }
    glow(955, 705, 110, 1.0f, 0.88f, 0.60f, 0.30f * moonA);
    glow(955, 705, 45, 1.0f, 0.95f, 0.75f, 0.35f * moonA);
    moon(955, 705, 26, moonA);
    cloud(150, 520, 1.2f, 1); cloud(fmod(380 + f * 0.15f, 1250) - 100, 570, 0.9f, 0.8f); cloud(800, 500, 1.0f, 1);
    for (int i = 0; i < 5; i++) {                                    // birds flying home
        float bxp = fmod(f * 1.1f + i * 34, 1300) - 100, byp = 640 + i * 9 + 6 * sin(f * 0.05f + i);
        bird(bxp, byp, 1.0f - i * 0.08f, 5 * sin(f * 0.4f + i));
    }

    if (titleA > 0) {                                                // fireworks after the moon is seen
        firework(250, 640, 82, 1.0f, 0.40f, 0.50f);
        firework(800, 690, 30, 0.40f, 0.80f, 1.0f);
        firework(520, 600, 120, 1.0f, 0.85f, 0.30f);
    }
    // ---- far city silhouette ----
    float sky1[14][2] = {{0,330},{60,380},{110,350},{170,410},{230,360},{300,420},{360,340},{430,390},
                         {500,350},{560,400},{620,370},{700,330},{760,360},{830,340}};
    for (int i = 0; i < 14; i++) {
        float x = sky1[i][0], w = (i < 13 ? sky1[i + 1][0] : 900) - x;
        col(0.42f, 0.27f, 0.42f); rect(x, 200, x + w + 2, sky1[i][1]);
        for (float wy = 230; wy < sky1[i][1] - 15; wy += 22)
            for (float wx = x + 8; wx < x + w - 8; wx += 16)
                if ((int)(wx * 3 + wy) % 5 < 2) { col(1, 0.82f, 0.5f, 0.55f); rect(wx, wy, wx + 4, wy + 6); }
    }

    // ---- left buildings with details ----
    struct B { float x1, x2, top, r, g, b; } bl[3] = {
        {0, 170, 640, 0.66f, 0.46f, 0.42f}, {170, 330, 560, 0.80f, 0.64f, 0.48f}, {330, 460, 480, 0.56f, 0.55f, 0.62f}};
    int idx = 0;
    for (int b = 0; b < 3; b++) {
        col(bl[b].r, bl[b].g, bl[b].b); rect(bl[b].x1, 200, bl[b].x2, bl[b].top);
        col(bl[b].r * 0.93f, bl[b].g * 0.93f, bl[b].b * 0.93f);
        for (float ly = 210; ly < bl[b].top; ly += 14) rect(bl[b].x1, ly, bl[b].x2, ly + 1);   // wall texture
        hgrad(bl[b].x2 - 30, 200, bl[b].x2, bl[b].top, 0, 0, 0, 0, 0, 0, 0, 0.25f);         // side shadow
        col(bl[b].r * 0.75f, bl[b].g * 0.75f, bl[b].b * 0.75f); rect(bl[b].x1, bl[b].top - 10, bl[b].x2, bl[b].top);
        for (float wy = 300; wy < bl[b].top - 60; wy += 90)
            for (float wx = bl[b].x1 + 24; wx < bl[b].x2 - 40; wx += 64, idx++)
                window(wx, wy, 34, 44, f > 30 + (idx * 41) % 200, true);
    }
    col(0.25f, 0.30f, 0.38f); rect(20, 640, 70, 672); col(0.20f, 0.24f, 0.30f); rect(20, 668, 70, 674);   // water tank
    col(0.72f, 0.72f, 0.74f); rect(140, 470, 162, 488); col(0.5f, 0.5f, 0.52f); for (int i = 0; i < 4; i++) rect(143, 473 + i * 4, 159, 474 + i * 4); // AC unit
    // rooftop people pointing at the moon (building 3)
    Look r1 = {0.20f, 0.20f, 0.16f, 0.28f, 0.22f, 0.20f, 0.30f, 0, true, false};
    Look r2 = {0.20f, 0.30f, 0.22f, 0.25f, 0.25f, 0.22f, 0.28f, 2, false, false};
    if (seen) {
        human(380, 480, 0.85f, r1, -10, 5, 135, 10);                        // pointing at the moon
        human(415, 480, 0.85f, r2, -150, 0, 150, 0, fabs(sin(f * 0.15f)) * 6);
    } else {                                                                 // searching the sky
        human(380, 480, 0.85f, r1, -10, 5, 160, -140);
        human(415, 480, 0.85f, r2, -160, 140, 10, -10);
    }
    // balcony family (building 1)
    col(0.32f, 0.24f, 0.24f); rect(14, 476, 150, 482);
    Look m1 = {0.70f, 0.95f, 0.95f, 0.90f, 0.90f, 0.90f, 0.86f, 0, true, true};
    Look w1 = {0.74f, 0.85f, 0.25f, 0.45f, 0.95f, 0.55f, 0.65f, 1, false, false};
    Look g1 = {0.70f, 1.0f, 0.75f, 0.25f, 0.95f, 0.45f, 0.30f, 3, false, false};
    float wave = 25 * sin(f * 0.15f);
    human(50, 482, 0.9f, m1, -8, 10, 150 + wave, 20);
    human(90, 482, 0.85f, w1, -8, 10, 8, -10);
    human(122, 482, 0.85f, g1, -150 - wave, 0, 150 + wave, 0);
    col(0.20f, 0.16f, 0.16f); for (float x = 16; x < 150; x += 8) rect(x, 482, x + 2, 520); rect(14, 518, 150, 522);  // railing
    col(0.25f, 0.55f, 0.30f); circle(26, 528, 9); circle(36, 530, 8); col(0.6f, 0.30f, 0.20f); rect(20, 520, 42, 528);   // plant pot

    // ground floor shop
    col(0.20f, 0.16f, 0.14f); rect(190, 200, 320, 290);
    grad(195, 200, 315, 280, 1.0f, 0.75f, 0.40f, 0.95f, 0.85f, 0.60f);
    col(0.55f, 0.35f, 0.20f); for (int s = 0; s < 3; s++) rect(198, 215 + s * 22, 312, 218 + s * 22);
    float jar[4][3] = {{0.9f,0.3f,0.3f},{0.3f,0.7f,0.4f},{0.95f,0.8f,0.2f},{0.3f,0.5f,0.9f}};
    for (int s = 0; s < 3; s++) for (int j = 0; j < 6; j++) { float* c = jar[(s + j) % 4]; col(c[0], c[1], c[2]); rect(204 + j * 18, 219 + s * 22, 214 + j * 18, 233 + s * 22); }
    col(0.55f, 0.55f, 0.58f); rect(190, 280, 320, 292);
    for (int s = 0; s < 4; s++) { col(0.45f, 0.45f, 0.48f); rect(190, 282 + s * 3, 320, 283 + s * 3); }
    col(0.10f, 0.45f, 0.35f); rect(185, 292, 325, 318); col(1, 1, 1); drawTextCentered(255, 300, "MAYER DOA STORE", GLUT_BITMAP_HELVETICA_12);
    glow(255, 250, 90, 1, 0.8f, 0.4f, 0.25f);

    // ---- mosque ----
    glow(900, 420, 330, 0.5f, 1.0f, 0.7f, 0.10f);
    col(0.36f, 0.58f, 0.56f); rect(720, 200, WIN_W, 440);
    hgrad(1040, 200, WIN_W, 440, 0, 0, 0, 0, 0, 0, 0, 0.25f);
    col(0.28f, 0.47f, 0.46f); rect(712, 436, WIN_W, 450);
    col(0.95f, 0.80f, 0.40f); for (float x = 720; x < WIN_W; x += 16) { triangle(x, 420, x + 8, 430, x + 16, 420); }   // pattern band
    glBegin(GL_TRIANGLE_FAN);                                            // shaded main dome
    col(0.62f, 0.82f, 0.78f); glVertex2f(890, 560);
    col(0.24f, 0.42f, 0.42f);
    for (int i = 0; i <= 40; i++) { float t = PI * i / 40; glVertex2f(940 + 140 * cos(t), 450 + 150 * sin(t)); }
    glVertex2f(1080, 450); glEnd();
    col(0.28f, 0.47f, 0.46f); rect(795, 450, 1085, 460);
    col(0.95f, 0.80f, 0.35f); rect(938, 600, 942, 625); circle(940, 628, 5); moon(940, 645, 11, 1);
    for (int i = 0; i < 2; i++) {                                         // small corner domes
        float cx = i ? 1080 : 760;
        col(0.32f, 0.52f, 0.50f); rect(cx - 18, 450, cx + 18, 480);
        col(0.26f, 0.44f, 0.44f); halfCircle(cx, 480, 20);
        col(0.95f, 0.80f, 0.35f); rect(cx - 1, 498, cx + 1, 512);
    }
    for (int d = 0; d < 3; d++) {                                         // arched doors with frames
        float x = 775 + d * 105;
        col(0.85f, 0.88f, 0.80f); archShape(x - 6, 200, x + 66, 360);
        col(0.16f, 0.26f, 0.30f); archShape(x, 200, x + 60, 352);
        grad(x + 6, 200, x + 54, 300, 1.0f, 0.80f, 0.45f, 0.95f, 0.70f, 0.35f, 0.8f, 0.2f);
    }
    col(0.92f, 0.95f, 0.85f); drawTextCentered(930, 395, "BAITUL AMAN JAME MASJID", GLUT_BITMAP_HELVETICA_12);
    fairyLights(712, 452, WIN_W, 452, -6, 22, 1);
    {   // lights along the dome
        col(0.1f, 0.1f, 0.1f, 0.5f);
        for (int i = 0; i <= 16; i++) {
            float t = PI * i / 16, x = 940 + 142 * cos(t), y = 452 + 152 * sin(t);
            bool on = (frameNo / 10 + i) % 2;
            if (on) glow(x, y, 8, 0.5f, 1, 0.6f, 0.6f);
            col(0.6f, 1, 0.65f, on ? 1 : 0.4f); circle(x, y, 2.4f);
        }
    }

    // ---- minaret with loudspeaker ----
    col(0.46f, 0.66f, 0.64f); rect(640, 200, 692, 650);
    hgrad(666, 200, 692, 650, 0, 0, 0, 0.05f, 0, 0, 0, 0.25f);
    col(0.30f, 0.48f, 0.47f);
    for (int b = 0; b < 3; b++) rect(634, 330 + b * 120, 698, 340 + b * 120);
    col(0.15f, 0.25f, 0.28f); for (int b = 0; b < 3; b++) archShape(658, 360 + b * 120, 674, 395 + b * 120);
    col(0.28f, 0.46f, 0.46f); rect(630, 640, 702, 652); halfCircle(666, 652, 28);
    col(0.95f, 0.80f, 0.35f); rect(665, 680, 667, 690);
    col(0.20f, 0.20f, 0.22f); rect(664, 690, 668, 712);
    col(0.85f, 0.86f, 0.88f);
    glBegin(GL_QUADS);
    glVertex2f(664, 708); glVertex2f(664, 718); glVertex2f(638, 728); glVertex2f(638, 698);
    glVertex2f(668, 708); glVertex2f(668, 718); glVertex2f(694, 728); glVertex2f(694, 698); glEnd();
    col(0.6f, 0.6f, 0.62f); rect(636, 698, 640, 728); rect(692, 698, 696, 728);
    glLineWidth(2.5f);
    for (int k = 0; k < 3; k++) {                                       // sound waves (scaling)
        float r = fmod(f * 0.8f + k * 20, 60) + 8;
        col(1, 1, 1, 1 - r / 70);
        arcLine(638, 713, r, r, PI * 0.72f, PI * 1.28f);
        arcLine(694, 713, r, r, -PI * 0.28f, PI * 0.28f);
    }
    glLineWidth(1);

    for (int i = 0; i < 6; i++) {                                    // sky lanterns floating up
        float xs[6] = {80, 230, 380, 530, 860, 1050};
        float ly = 470 + fmod(f * 0.45f + i * 45, 250), lx = xs[i] + 18 * sin(f * 0.02f + i);
        if (lx > 640 && ly < 610) ly += 110;                           // keep them above the dome
        fanush(lx, ly, 0.8f + (i % 3) * 0.15f);
    }
    // ---- electric pole, wires and buntings ----
    col(0.22f, 0.20f, 0.22f); rect(500, 190, 508, 600); rect(480, 580, 528, 586);
    col(0.30f, 0.30f, 0.34f); rect(509, 470, 530, 500);
    col(0.08f, 0.08f, 0.10f, 0.8f);
    for (int w = 0; w < 3; w++) {
        glBegin(GL_LINE_STRIP);
        for (int i = 0; i <= 20; i++) { float t = i / 20.0f; glVertex2f(mix(170, 504, t), mix(560 - w * 8, 584, t) - 18 * 4 * t * (1 - t)); }
        glEnd();
        glBegin(GL_LINE_STRIP);
        for (int i = 0; i <= 20; i++) { float t = i / 20.0f; glVertex2f(mix(504, 720, t), mix(584, 440 + w * 6, t) - 14 * 4 * t * (1 - t)); }
        glEnd();
    }
    bunting(0, 620, 640, 640, 80, 0, 22);
    bunting(170, 548, 640, 590, 60, 2, 18);
    bunting(330, 470, 640, 520, 40, 4, 14);
    bunting(692, 640, WIN_W, 600, 55, 1, 14);
    fairyLights(0, 590, 504, 584, 40, 18, 0);

    // ---- street ----
    grad(0, 0, WIN_W, 182, 0.20f, 0.19f, 0.25f, 0.34f, 0.30f, 0.36f);
    col(0.56f, 0.50f, 0.50f); rect(0, 182, WIN_W, 200);
    col(0.45f, 0.40f, 0.42f); rect(0, 180, WIN_W, 184);
    col(0.85f, 0.85f, 0.80f, 0.5f); for (float x = 20; x < WIN_W; x += 90) rect(x, 88, x + 45, 92);   // lane marks
    glow(560, 360, 120, 1, 0.75f, 0.4f, 0.18f);
    col(0.15f, 0.15f, 0.18f); rect(578, 196, 584, 360); rect(560, 356, 584, 361);       // street lamp
    glow(562, 350, 70, 1, 0.85f, 0.5f, 0.35f); col(1, 0.92f, 0.65f); ellipse(562, 350, 7, 4);
    grad(520, 182, 610, 200, 1, 0.8f, 0.5f, 1, 0.8f, 0.5f, 0.0f, 0.0f);

    rickshaw(880, 160);
    Look pu = {0.55f, 0.30f, 0.55f, 0.40f, 0.25f, 0.45f, 0.30f, 0, false, false};    // rickshaw puller
    human(830, 165, 1.25f, pu, -10, 20, 60, 40);
    col(0.90f, 0.30f, 0.20f); rect(823, 230, 829, 245);                                // gamchha on the shoulder
    teaStall(370, 190);
    Look tm = {0.64f, 0.95f, 0.90f, 0.80f, 0.25f, 0.30f, 0.40f, 0, true, false};
    human(475, 186, 1.2f, tm, -10, 20, 25, 120);                                       // man with a cup of tea
    col(0.95f, 0.95f, 0.92f); rect(487, 260, 494, 268);
    balloonSeller(290, 96);
    Look imam = {0.68f, 0.96f, 0.96f, 0.93f, 0.93f, 0.93f, 0.90f, 0, true, true};
    if (seen) human(610, 186, 1.35f, imam, -10, 30, 140, 15);           // pointing up at the moon
    else      human(610, 186, 1.35f, imam, -10, 30, 160, -145);         // hand over the eyes, searching
    Look man2 = {0.60f, 0.35f, 0.55f, 0.45f, 0.20f, 0.30f, 0.45f, 0, false, false};
    human(735, 186, 1.3f, man2, -20, 40, 20, -40);
    // kids celebrating with sparklers
    Look k1 = {0.62f, 0.95f, 0.45f, 0.25f, 0.25f, 0.30f, 0.50f, 2, false, false};
    Look k2 = {0.66f, 0.92f, 0.30f, 0.45f, 0.95f, 0.75f, 0.30f, 3, false, false};
    Look k3 = {0.58f, 0.30f, 0.72f, 0.55f, 0.92f, 0.92f, 0.90f, 2, true, false};
    float j1 = fabs(sin(f * 0.12f)) * 16, j2 = fabs(sin(f * 0.12f + 1.8f)) * 16;
    if (seen) {                                                         // jumping with joy, sparklers lit
        human(80, 110, 1.3f, k1, -150, 0, 150, 0, j1);
        human(150, 106, 1.3f, k2, -30, 60, 50, 40);
        sparkler(168, 128, 1);
        human(220, 112, 1.3f, k3, -150, 0, 30, 50, j2);
        sparkler(236, 140 + j2, 2);
    } else {                                                            // waiting and looking up
        human(80, 110, 1.3f, k1, -10, 10, 160, -140);
        human(150, 106, 1.3f, k2, -10, 10, 10, -10);
        human(220, 112, 1.3f, k3, -160, 140, 10, -10);
    }
    // boy on a bicycle looking at the moon
    bicycleRider(fmod(f * 1.6f, WIN_W + 300) - 150, 30);

    // ---- speech bubbles from the mosque speaker ----
    if (f > 190) {
        bubble(355, 650, 632, 708, "Eid er chand dekha geche!", "Eid Mubarak! Eid Mubarak!");
        bubble(722, 640, 700, 706, "Agamikal Eid-er jamaat", 0);
    }
    // ---- soft vignette for a cinematic look ----
    hgrad(0, 0, 80, WIN_H, 0, 0, 0, 0.35f, 0, 0, 0, 0);
    hgrad(WIN_W - 80, 0, WIN_W, WIN_H, 0, 0, 0, 0, 0, 0, 0, 0.35f);
    grad(0, 0, WIN_W, 60, 0, 0, 0, 0, 0, 0, 0.35f, 0);

    // ---- title ----
    if (titleA > 0) {
        col(0.30f, 0.12f, 0.20f, 0.6f * titleA); strokeText(553, 737, "EID MUBARAK", 0.36f, 5);
        col(1.0f, 0.84f, 0.35f, titleA); strokeText(550, 740, "EID MUBARAK", 0.36f, 4);
    }

    // ---- slide label (top left) ----
    col(0.08f, 0.06f, 0.16f, 0.75f); roundRect(14, 752, 200, 786, 14);
    col(1.0f, 0.82f, 0.35f); circle(36, 769, 10);
    col(0.15f, 0.10f, 0.25f); drawTextCentered(36, 764, "1", GLUT_BITMAP_HELVETICA_12);
    col(1, 1, 1); drawText(54, 764, "Chand Raat", GLUT_BITMAP_HELVETICA_18);

    // ---- story caption (bottom) ----
    const char* cap = f < 60  ? "The sun has set. Everyone is waiting..."
                    : f < 180 ? "People search the sky for the thin Eid moon..."
                    : f < 300 ? "There it is! The Eid moon is seen!"
                    :           "Eid Mubarak! Tomorrow is Eid.";
    float cw = textW(cap, GLUT_BITMAP_HELVETICA_18);
    col(0.06f, 0.05f, 0.12f, 0.78f); roundRect(550 - cw / 2 - 22, 32, 550 + cw / 2 + 22, 64, 16);
    col(1.0f, 0.92f, 0.70f); drawTextCentered(550, 42, cap, GLUT_BITMAP_HELVETICA_18);
}