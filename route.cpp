// ============================================================
//  route.cpp  -  road graph, Dijkstra, visiting order, panel
//  Owner: Arifa
// ============================================================
#include "common.h"
#include <algorithm>              // next_permutation

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

// ------------------------------------------------------------
//  GRAPH + DIJKSTRA
// ------------------------------------------------------------
const double INF = 1e18;
const int    EIDGAH_ROAD = 4;           // edge F-G, closed during Eid prayer
const double WALK_M_PER_MIN = 75.0;     // 4.5 km/h = 75 metres per minute
double adj[N][N];                       // adjacency matrix (metres), INF = no road

void buildGraph() {
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++) adj[i][j] = (i == j) ? 0 : INF;
    for (int k = 0; k < E_COUNT; k++) {
        if (edges[k].closed) continue;               // closed road = no connection
        double d = nodeDist(edges[k].u, edges[k].v);
        adj[edges[k].u][edges[k].v] = d;
        adj[edges[k].v][edges[k].u] = d;             // two-way road
    }
}

// Dijkstra: shortest distance from src to dst, also fills the node path
// returns -1 if dst cannot be reached
double dijkstra(int src, int dst, vector<int>& path) {
    double dist[N]; int prev[N]; bool done[N];
    for (int i = 0; i < N; i++) { dist[i] = INF; prev[i] = -1; done[i] = false; }
    dist[src] = 0;
    for (int it = 0; it < N; it++) {
        int u = -1;                                  // closest node not finished yet
        for (int i = 0; i < N; i++)
            if (!done[i] && (u == -1 || dist[i] < dist[u])) u = i;
        if (u == -1 || dist[u] >= INF) break;
        done[u] = true;
        for (int v = 0; v < N; v++)                  // relax the neighbours
            if (adj[u][v] < INF && dist[u] + adj[u][v] < dist[v]) {
                dist[v] = dist[u] + adj[u][v];
                prev[v] = u;
            }
    }
    path.clear();
    if (dist[dst] >= INF) return -1;
    for (int v = dst; v != -1; v = prev[v]) path.insert(path.begin(), v);
    return dist[dst];
}

// ------------------------------------------------------------
//  VISITING ORDER (three methods)
// ------------------------------------------------------------
int  sender = 2;                        // House 3 sends the gifts
bool receiver[HN] = {false};            // houses that will receive a gift

double hd[HN][HN];                      // shortest distance house i -> house j
vector<int> hp[HN][HN];                 // shortest node path house i -> house j

struct Plan {
    vector<int> order;                  // houses in visiting order
    vector<int> path;                   // full node path to walk
    double m;                           // total metres, -1 = no plan
    Plan() : m(-1) {}
};
Plan planNumber, planNearest, planOptimal;
long ordersChecked = 0;

void computeHouseDistances() {
    buildGraph();
    for (int i = 0; i < HN; i++)
        for (int j = 0; j < HN; j++) {
            if (i == j) { hd[i][j] = 0; hp[i][j].assign(1, houses[i].node); }
            else hd[i][j] = dijkstra(houses[i].node, houses[j].node, hp[i][j]);
        }
}

double orderCost(const vector<int>& order) {
    double s = 0; int cur = sender;
    for (size_t k = 0; k < order.size(); k++) {
        if (hd[cur][order[k]] < 0) return INF;
        s += hd[cur][order[k]];
        cur = order[k];
    }
    return s;
}

void makePlan(Plan& p, const vector<int>& order) {
    p = Plan();
    p.order = order;
    if (orderCost(order) >= INF) return;
    p.m = 0;
    int cur = sender;
    p.path.push_back(houses[sender].node);
    for (size_t k = 0; k < order.size(); k++) {
        p.m += hd[cur][order[k]];
        const vector<int>& seg = hp[cur][order[k]];
        for (size_t s = 1; s < seg.size(); s++) p.path.push_back(seg[s]);
        cur = order[k];
    }
}

void computePlans() {
    computeHouseDistances();
    planNumber = planNearest = planOptimal = Plan();
    ordersChecked = 0;
    vector<int> targets;
    for (int i = 0; i < HN; i++) if (receiver[i] && i != sender) targets.push_back(i);
    if (targets.empty()) return;

    // Method 1: by house number - O(n)
    makePlan(planNumber, targets);

    // Method 2: nearest neighbour - always go to the closest remaining house, O(n^2)
    vector<int> left = targets, nn;
    int cur = sender;
    while (!left.empty()) {
        int best = -1;
        for (size_t k = 0; k < left.size(); k++) {
            if (hd[cur][left[k]] < 0) continue;
            if (best == -1 || hd[cur][left[k]] < hd[cur][left[best]]) best = (int)k;
        }
        if (best == -1) { nn.insert(nn.end(), left.begin(), left.end()); break; }
        nn.push_back(left[best]);
        cur = left[best];
        left.erase(left.begin() + best);
    }
    makePlan(planNearest, nn);

    // Method 3: optimal - try every possible order (brute force TSP), O(n!)
    vector<int> perm = targets, bestOrder = targets;
    double bestCost = INF;
    do {
        ordersChecked++;
        double c = orderCost(perm);
        if (c < bestCost) { bestCost = c; bestOrder = perm; }
    } while (next_permutation(perm.begin(), perm.end()));
    makePlan(planOptimal, bestOrder);
}

void routeInit() {
    computePlans();
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

    for (int k = 0; k < E_COUNT; k++) {               // closed road: red cover and a no-entry sign
        if (!edges[k].closed) continue;
        Node a = nodes[edges[k].u], b = nodes[edges[k].v];
        glColor4f(0.85f, 0.10f, 0.10f, 0.45f);
        thickLine(a.x, a.y, b.x, b.y, 26);
        float mx = (a.x + b.x) / 2 - 40, my = (a.y + b.y) / 2;
        setColor(0.85f, 0.10f, 0.10f); circle(mx, my, 15);
        setColor(1, 1, 1);             rect(mx - 9, my - 3, mx + 9, my + 3);
    }
}

// draw a route as a line through its nodes (off = small shift so two routes do not overlap)
void drawPlanPath(const Plan& p, float r, float g, float b, float w, bool dashed, float off) {
    if (p.m < 0 || p.path.size() < 2) return;
    glColor4f(r, g, b, 0.95f);
    glLineWidth(w);
    if (dashed) { glEnable(GL_LINE_STIPPLE); glLineStipple(3, 0xAAAA); }
    glBegin(GL_LINE_STRIP);
    for (size_t i = 0; i < p.path.size(); i++) glVertex2f(nodes[p.path[i]].x + off, nodes[p.path[i]].y + off);
    glEnd();
    if (dashed) glDisable(GL_LINE_STIPPLE);
    glLineWidth(1);
}

// arrows above houses (blue = sender, red = receiver) and visiting order badges
void drawHouseMarks() {
    for (int i = 0; i < HN; i++) {
        float cx = houses[i].hx + HOUSE_W / 2, top = houses[i].hy + HOUSE_H + 34;
        if (i == sender) {
            setColor(0.20f, 0.50f, 1.0f);
            triangle(cx - 9, top + 16, cx + 9, top + 16, cx, top);
            drawText(cx - 14, top + 24, "FROM", GLUT_BITMAP_HELVETICA_10);
        } else if (receiver[i]) {
            float bob = 3 * sin(frameNo * 0.12f);
            setColor(1.0f, 0.25f, 0.25f);
            triangle(cx - 9, top + 16 + bob, cx + 9, top + 16 + bob, cx, top + bob);
        }
    }
    for (size_t k = 0; k < planOptimal.order.size(); k++) {       // 1st, 2nd, 3rd ... stop
        const House& h = houses[planOptimal.order[k]];
        setColor(0.15f, 0.20f, 0.35f); circle(h.hx - 4, h.hy + HOUSE_H + 12, 10);
        setColor(1.0f, 0.85f, 0.25f);  circle(h.hx - 4, h.hy + HOUSE_H + 12, 8.5f);
        setColor(0.1f, 0.1f, 0.1f);
        char t[16]; sprintf(t, "%d", (int)k + 1);
        drawText(h.hx - 7, h.hy + HOUSE_H + 8, t);
    }
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
    drawPlanPath(planNumber, 1.0f, 0.55f, 0.10f, 5, true, 6);     // orange dashed = house number order
    drawPlanPath(planOptimal, 0.10f, 0.85f, 0.35f, 7, false, 0);  // green = optimal route
    drawRoadLengths();
    drawNodes();
    drawHouses();
    drawHouseMarks();
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

    char buf[80];
    float y = panelCard(WIN_H - 82, 106, "ROAD NETWORK");
    sprintf(buf, "%d", N);                      panelRow(y, "Intersections", buf);
    sprintf(buf, "%d", E_COUNT);                panelRow(y - 17, "Roads", buf);
    sprintf(buf, "%.0f m", totalRoadLength());  panelRow(y - 34, "Total length", buf);
    panelRow(y - 51, "Eidgah road F-G", edges[EIDGAH_ROAD].closed ? "CLOSED" : "open");

    // who sends, who receives
    y = panelCard(WIN_H - 200, 71, "GIFT EXCHANGE");
    sprintf(buf, "House %d", sender + 1);       panelRow(y, "From", buf);
    char to[40] = ""; int cnt = 0;
    for (int i = 0; i < HN; i++)
        if (receiver[i] && i != sender) { char t[16]; sprintf(t, cnt ? ", %d" : "%d", i + 1); strcat(to, t); cnt++; }
    panelRow(y - 17, "To", cnt ? to : "press 1-5");

    // the three methods
    y = panelCard(WIN_H - 283, 190, "VISITING ORDER");
    const char* names[3] = {"By house number", "Nearest neighbour", "Optimal (all orders)"};
    float mc[3][3] = {{1.0f, 0.60f, 0.20f}, {0.30f, 0.85f, 0.95f}, {0.30f, 0.90f, 0.45f}};
    Plan* plans[3] = {&planNumber, &planNearest, &planOptimal};
    for (int m = 0; m < 3; m++) {
        float yy = y - m * 52;
        setColor(mc[m][0], mc[m][1], mc[m][2]);
        rect(824, yy + 1, 830, yy + 9);
        drawText(836, yy, names[m], GLUT_BITMAP_HELVETICA_12);
        if (!cnt) continue;
        Plan& p = *plans[m];
        if (p.m < 0) { panelRow(yy - 17, "", "not possible"); continue; }
        sprintf(buf, "%d", sender + 1);
        for (size_t k = 0; k < p.order.size(); k++) { char t[16]; sprintf(t, " > %d", p.order[k] + 1); strcat(buf, t); }
        setColor(0.85f, 0.88f, 0.95f); drawText(836, yy - 17, buf, GLUT_BITMAP_HELVETICA_12);
        sprintf(buf, "%.0f m  |  %.1f min", p.m, p.m / WALK_M_PER_MIN);
        setColor(1, 1, 1); drawText(836, yy - 33, buf, GLUT_BITMAP_HELVETICA_12);
    }

    // how much the optimal order saves
    y = panelCard(WIN_H - 485, 105, "RESULT");
    if (cnt && planNumber.m > 0 && planOptimal.m >= 0) {
        double saved = planNumber.m - planOptimal.m;
        sprintf(buf, "%.0f m  (%.1f%%)", saved, saved / planNumber.m * 100);
        panelRow(y, "Optimal saves", buf);
        sprintf(buf, "%.1f min", saved / WALK_M_PER_MIN);  panelRow(y - 17, "Time saved", buf);
        sprintf(buf, "%ld", ordersChecked);                 panelRow(y - 34, "Orders checked", buf);
    } else {
        setColor(0.70f, 0.74f, 0.82f);
        drawText(824, y, "Choose receiver houses", GLUT_BITMAP_HELVETICA_12);
    }

    // map legend
    y = 186;
    glLineWidth(6); setColor(0.10f, 0.85f, 0.35f);
    glBegin(GL_LINES); glVertex2f(824, y + 4); glVertex2f(852, y + 4); glEnd();
    glLineWidth(4); setColor(1.0f, 0.55f, 0.10f);
    glEnable(GL_LINE_STIPPLE); glLineStipple(3, 0xAAAA);
    glBegin(GL_LINES); glVertex2f(824, y - 14); glVertex2f(852, y - 14); glEnd();
    glDisable(GL_LINE_STIPPLE); glLineWidth(1);
    setColor(0.85f, 0.88f, 0.95f);
    drawText(862, y, "Optimal route", GLUT_BITMAP_HELVETICA_12);
    drawText(862, y - 18, "House number order", GLUT_BITMAP_HELVETICA_12);

    // controls
    y = panelCard(150, 130, "CONTROLS");
    const char* help[6] = {"1-5 / left click   add or remove receiver", "F / right click     change sender",
                           "X     close / open Eidgah road", "N     day / night",
                           "R     reset", "ESC  exit"};
    setColor(0.80f, 0.83f, 0.90f);
    for (int i = 0; i < 6; i++) drawText(824, y - i * 15, help[i], GLUT_BITMAP_HELVETICA_10);
}

void resetRoute() {
    for (int i = 0; i < HN; i++) receiver[i] = false;
    sender = 2;
    edges[EIDGAH_ROAD].closed = false;
    computePlans();
}

void toggleReceiver(int i) {
    if (i == sender) return;
    receiver[i] = !receiver[i];
    computePlans();
}

void setSender(int i) {
    sender = i;
    receiver[i] = false;                // a house does not send to itself
    computePlans();
}

void routeKey(unsigned char key) {
    if (key >= '1' && key <= '5') { toggleReceiver(key - '1'); return; }
    if (key == 'f' || key == 'F') setSender((sender + 1) % HN);
    if (key == 'x' || key == 'X') { edges[EIDGAH_ROAD].closed = !edges[EIDGAH_ROAD].closed; computePlans(); }
    if (key == 'r' || key == 'R') resetRoute();
}

// click on a house: left = receiver on/off, right = make it the sender
void routeMouse(int button, float wx, float wy) {
    for (int i = 0; i < HN; i++) {
        const House& h = houses[i];
        if (wx >= h.hx - 6 && wx <= h.hx + HOUSE_W + 6 && wy >= h.hy && wy <= h.hy + HOUSE_H + 26) {
            if (button == GLUT_RIGHT_BUTTON) setSender(i);
            else if (button == GLUT_LEFT_BUTTON) toggleReceiver(i);
            return;
        }
    }
}

void routeUpdate() {
    // TODO (Arifa): move the gift carrier along the route
}