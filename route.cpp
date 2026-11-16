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

void routeInit() {
    // TODO (Arifa): build the road graph for Dijkstra
}

// grey roads with a dashed white centre line
void drawRoads() {
    float rc = night ? 0.22f : 0.35f;
    setColor(rc, rc, rc + 0.03f);
    for (int k = 0; k < E_COUNT; k++) {
        Node a = nodes[edges[k].u], b = nodes[edges[k].v];
        thickLine(a.x, a.y, b.x, b.y, 26);
    }
    for (int i = 0; i < N; i++) circle(nodes[i].x, nodes[i].y, 13);   // round corners

    setColor(0.95f, 0.95f, 0.85f);
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
    // TODO (Arifa): houses, routes
}

void drawPanel() {
    setColor(0.11f, 0.13f, 0.21f);              // panel background
    rect(800, 0, WIN_W, WIN_H);
    setColor(1.0f, 0.82f, 0.3f);
    rect(800, 0, 803, WIN_H);                   // gold border
    drawText(818, WIN_H - 35, "EidVille", GLUT_BITMAP_HELVETICA_18);
    setColor(1, 1, 1);
    drawText(818, WIN_H - 60, "Smart Eid Gift Exchange", GLUT_BITMAP_HELVETICA_12);

    char buf[60];
    sprintf(buf, "Road network: %d intersections, %d roads", N, E_COUNT);
    drawText(818, WIN_H - 95, buf);
    sprintf(buf, "Total road length: %.0f m", totalRoadLength());
    drawText(818, WIN_H - 113, buf);

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