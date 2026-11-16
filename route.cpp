// ============================================================
//  route.cpp  -  road graph, Dijkstra, visiting order, panel
//  Owner: Arifa
// ============================================================
#include "common.h"

float walkerX = 80, walkerY = 120, walkerAngle = 0;
bool  walking = false;
int   giftsLeft = 0;

// ------------------------------------------------------------
//  ROAD NETWORK
//  Each intersection is a node, each road is an edge.
//  1 pixel = 1 metre.
// ------------------------------------------------------------
const int N = 8;                       // number of intersections

struct Node { char name; float x, y; };

Node nodes[N] = {
    {'A',  80, 120}, {'B', 280, 120}, {'C', 480, 120}, {'D', 720, 120},
    {'E',  80, 380}, {'F', 280, 380}, {'G', 480, 380}, {'H', 720, 380}
};

struct Edge { int u, v; bool closed; };

Edge edges[] = {
    {0,1,false}, {1,2,false}, {2,3,false},              // bottom road  A-B-C-D
    {4,5,false}, {5,6,false}, {6,7,false},              // top road     E-F-G-H (F-G = Eidgah road)
    {0,4,false}, {1,5,false}, {2,6,false}, {3,7,false}, // vertical roads
    {1,6,false}                                         // diagonal shortcut B-G
};
const int E_COUNT = sizeof(edges) / sizeof(edges[0]);

// length of the road between two intersections (metres)
// Euclidean distance: sqrt((x1-x2)^2 + (y1-y2)^2)
double nodeDist(int a, int b) {
    double dx = nodes[a].x - nodes[b].x, dy = nodes[a].y - nodes[b].y;
    return sqrt(dx * dx + dy * dy);
}

double totalRoadLength() {
    double s = 0;
    for (int k = 0; k < E_COUNT; k++) s += nodeDist(edges[k].u, edges[k].v);
    return s;
}

// ------------------------------------------------------------
//  HOUSES
//  Five houses, each standing beside one intersection.
// ------------------------------------------------------------
struct House {
    const char* name;
    int   node;            // road intersection beside the house
    float hx, hy;          // bottom-left corner of the house
    float r, g, b;         // wall colour
};

const int HN = 5;
House houses[HN] = {
    {"House 1", 2, 500, 140, 0.95f, 0.55f, 0.55f},   // beside C
    {"House 2", 3, 740, 140, 0.55f, 0.70f, 0.95f},   // beside D
    {"House 3", 5, 300, 400, 0.95f, 0.80f, 0.40f},   // beside F
    {"House 4", 6, 500, 400, 0.70f, 0.55f, 0.90f},   // beside G
    {"House 5", 7, 740, 400, 0.50f, 0.85f, 0.65f}    // beside H
};
const float HOUSE_W = 55, HOUSE_H = 42;

// soft shadow on the ground (this file only)
static void groundShadow(float cx, float cy, float rx, float ry) {
    glColor4f(0, 0, 0, 0.20f);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx, cy);
    for (int i = 0; i <= 40; i++) {
        float t = 2 * PI * i / 40;
        glVertex2f(cx + rx * cos(t), cy + ry * sin(t));
    }
    glEnd();
}

// house: shaded wall, two-tone roof, chimney, door, framed windows and its number
void drawHouse(const House& h, int number) {
    float k = night ? 0.6f : 1.0f;                   // darker at night
    float x = h.hx, y = h.hy, W = HOUSE_W, H = HOUSE_H;
    groundShadow(x + W / 2 + 4, y, W * 0.62f, 6);

    setColor(h.r * k, h.g * k, h.b * k);             // wall (front)
    rect(x, y, x + W, y + H);
    setColor(h.r * k * 0.82f, h.g * k * 0.82f, h.b * k * 0.82f);   // wall (side in shade)
    rect(x + W * 0.72f, y, x + W, y + H);

    setColor(0.45f * k, 0.25f * k, 0.20f * k);       // chimney
    rect(x + W - 16, y + H + 8, x + W - 8, y + H + 24);
    setColor(0.72f * k, 0.18f * k, 0.16f * k);       // roof (lit half)
    triangle(x - 6, y + H, x + W / 2, y + H, x + W / 2, y + H + 26);
    setColor(0.55f * k, 0.12f * k, 0.12f * k);       // roof (shaded half)
    triangle(x + W / 2, y + H, x + W + 6, y + H, x + W / 2, y + H + 26);

    setColor(0.40f, 0.22f, 0.10f);                   // door with step
    rect(x + 22, y, x + 33, y + 20);
    setColor(0.85f * k, 0.85f * k, 0.82f * k);
    rect(x + 19, y, x + 36, y + 2);
    setColor(0.95f * k, 0.95f * k, 0.95f * k);       // window frames
    rect(x + 5, y + 23, x + 19, y + 35);
    rect(x + 36, y + 23, x + 50, y + 35);
    if (night) setColor(1.0f, 0.86f, 0.40f);         // windows glow at night
    else       setColor(0.62f, 0.82f, 0.95f);
    rect(x + 6, y + 24, x + 18, y + 34);
    rect(x + 37, y + 24, x + 49, y + 34);
    setColor(0.95f * k, 0.95f * k, 0.95f * k);       // window cross bars
    rect(x + 11.5f, y + 24, x + 12.5f, y + 34);
    rect(x + 42.5f, y + 24, x + 43.5f, y + 34);
    setColor(1, 1, 1);                               // house number on the roof
    char lbl[4];
    sprintf(lbl, "%d", number);
    drawText(x + HOUSE_W / 2 - 3, y + HOUSE_H + 6, lbl);
}

void drawHouses() {
    for (int i = 0; i < HN; i++) drawHouse(houses[i], i + 1);
}
void routeInit() {
    // TODO (Arifa): build the road graph for Dijkstra
}

// roads: light footpath edge, dark asphalt, dashed centre line
void drawRoads() {
    float pv = night ? 0.45f : 0.78f;                 // footpath / kerb
    setColor(pv, pv, pv * 0.96f);
    for (int k = 0; k < E_COUNT; k++) {
        Node a = nodes[edges[k].u], b = nodes[edges[k].v];
        thickLine(a.x, a.y, b.x, b.y, 34);
    }
    for (int i = 0; i < N; i++) circle(nodes[i].x, nodes[i].y, 17);

    float rc = night ? 0.18f : 0.30f;                 // asphalt
    setColor(rc, rc, rc + 0.03f);
    for (int k = 0; k < E_COUNT; k++) {
        Node a = nodes[edges[k].u], b = nodes[edges[k].v];
        thickLine(a.x, a.y, b.x, b.y, 26);
    }
    for (int i = 0; i < N; i++) circle(nodes[i].x, nodes[i].y, 13);   // round corners

    setColor(1.0f, 0.88f, 0.45f);                     // yellow centre line
    glEnable(GL_LINE_STIPPLE);
    glLineStipple(2, 0x00FF);
    glLineWidth(1.5f);
    glBegin(GL_LINES);
    for (int k = 0; k < E_COUNT; k++) {
        glVertex2f(nodes[edges[k].u].x, nodes[edges[k].u].y);
        glVertex2f(nodes[edges[k].v].x, nodes[edges[k].v].y);
    }
    glEnd();
    glDisable(GL_LINE_STIPPLE);
    glLineWidth(1);
}

// small white circle with the intersection's letter
void drawNodes() {
    for (int i = 0; i < N; i++) {
        setColor(0.15f, 0.20f, 0.35f);              // dark ring
        circle(nodes[i].x, nodes[i].y, 10);
        setColor(1, 1, 1);
        circle(nodes[i].x, nodes[i].y, 8);
        setColor(0.1f, 0.1f, 0.1f);
        char s[2] = {nodes[i].name, 0};
        drawText(nodes[i].x - 4, nodes[i].y - 4, s);
    }
}

// road length written in a small box at the middle of each road
void drawRoadLengths() {
    for (int k = 0; k < E_COUNT; k++) {
        Node a = nodes[edges[k].u], b = nodes[edges[k].v];
        float mx = (a.x + b.x) / 2, my = (a.y + b.y) / 2;
        if (k == 10) { mx -= 22; my += 8; }       // diagonal: move label off the line
        char buf[16];
        sprintf(buf, "%.0f m", nodeDist(edges[k].u, edges[k].v));
        int w = 0;
        for (char* c = buf; *c; c++) w += glutBitmapWidth(GLUT_BITMAP_HELVETICA_10, *c);
        setColor(0.15f, 0.20f, 0.35f);            // label border
        rect(mx - w / 2.0f - 4, my - 7, mx + w / 2.0f + 4, my + 9);
        setColor(1, 1, 1);
        rect(mx - w / 2.0f - 3, my - 6, mx + w / 2.0f + 3, my + 8);
        setColor(0.15f, 0.15f, 0.2f);
        drawText(mx - w / 2.0f, my - 2, buf, GLUT_BITMAP_HELVETICA_10);
    }
}

void drawRoadLayer() {
    drawRoads();
    drawRoadLengths();
    drawNodes();
    drawHouses();
    // TODO (Arifa): routes
}

// ---------- panel helpers (this file only) ----------
// a card = slightly lighter box with a gold title, returns y below the title
static float panelCard(float y0, float h, const char* title) {
    setColor(0.16f, 0.19f, 0.29f);
    rect(814, y0 - h, WIN_W - 14, y0);
    setColor(1.0f, 0.82f, 0.3f);
    rect(814, y0 - 2, WIN_W - 14, y0);              // gold line on top of the card
    drawText(824, y0 - 20, title, GLUT_BITMAP_HELVETICA_12);
    return y0 - 40;
}

// one row: label on the left, value on the right
static void panelRow(float y, const char* label, const char* value) {
    setColor(0.70f, 0.74f, 0.82f);
    drawText(824, y, label, GLUT_BITMAP_HELVETICA_12);
    int w = 0;
    for (const char* c = value; *c; c++) w += glutBitmapWidth(GLUT_BITMAP_HELVETICA_12, *c);
    setColor(1, 1, 1);
    drawText(WIN_W - 24 - w, y, value, GLUT_BITMAP_HELVETICA_12);
}

void drawPanel() {
    setColor(0.10f, 0.12f, 0.19f);              // panel background
    rect(800, 0, WIN_W, WIN_H);
    setColor(0.07f, 0.09f, 0.15f);              // header bar
    rect(800, WIN_H - 70, WIN_W, WIN_H);
    setColor(1.0f, 0.82f, 0.3f);
    rect(800, 0, 803, WIN_H);                   // gold border
    drawText(818, WIN_H - 32, "EidVille", GLUT_BITMAP_TIMES_ROMAN_24);
    setColor(0.75f, 0.80f, 0.90f);
    drawText(818, WIN_H - 55, "Smart Eid Gift Exchange Simulator", GLUT_BITMAP_HELVETICA_12);

    char buf[40];
    float y = panelCard(WIN_H - 85, 100, "ROAD NETWORK");
    sprintf(buf, "%d", N);                      panelRow(y, "Intersections", buf);
    sprintf(buf, "%d", E_COUNT);                panelRow(y - 18, "Roads", buf);
    sprintf(buf, "%.0f m", totalRoadLength());  panelRow(y - 36, "Total length", buf);
    sprintf(buf, "%d", HN);                     panelRow(y - 54, "Houses", buf);

    // TODO (Arifa): visiting order, distance, time, comparison
}

void routeKey(unsigned char key) {
    (void)key;
    // TODO (Arifa): 1-5 receivers, F sender, S / G / A start, X road closure, R reset
}

void routeMouse(int button, float wx, float wy) {
    (void)button; (void)wx; (void)wy;
    // TODO (Arifa): click on a house to select it
}

void routeUpdate() {
    // TODO (Arifa): move the gift carrier along the route
}