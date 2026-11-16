// ============================================================
//  route.cpp  -  road graph, Dijkstra, visiting order, panel
//  Owner: Arifa
// ============================================================
#include "common.h"

float walkerX = 80, walkerY = 120, walkerAngle = 0;
bool  walking = false;

void routeInit() {
    // TODO (Arifa): build the road graph
}

void drawRoadLayer() {
    // TODO (Arifa): roads, houses, routes
}

void drawPanel() {
    setColor(0.11f, 0.13f, 0.21f);              // panel background
    rect(800, 0, WIN_W, WIN_H);
    setColor(1.0f, 0.82f, 0.3f);
    rect(800, 0, 803, WIN_H);                   // gold border
    drawText(818, WIN_H - 35, "EidVille", GLUT_BITMAP_HELVETICA_18);
    setColor(1, 1, 1);
    drawText(818, WIN_H - 60, "Smart Eid Gift Exchange", GLUT_BITMAP_HELVETICA_12);

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
