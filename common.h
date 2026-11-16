// ============================================================
//  common.h  -  shared settings and function list
//  EidVille: Smart Eid Gift Exchange & Route Optimization
//
//  File owners:
//    main.cpp   -> window, timer, helpers (shared, change rarely)
//    scene.cpp  -> Ijma  : Eid town graphics and animation
//    route.cpp  -> Arifa : road graph, Dijkstra, visiting order, panel
// ============================================================
#ifndef COMMON_H
#define COMMON_H

#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/glut.h>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
using namespace std;

// ---------- window ----------
const int   WIN_W = 1100, WIN_H = 800;   // world size (town 0-800, panel 800-1100)
const int   SCR_W = 935,  SCR_H = 680;   // real window size in pixels
const float PI = 3.14159265f;

// ---------- shared state ----------
extern int   frameNo;                    // main.cpp  : increases every frame
extern bool  night;                      // scene.cpp : day / night
extern float walkerX, walkerY;           // route.cpp : position of the gift carrier
extern float walkerAngle;                // route.cpp : walking direction (degree)
extern bool  walking;                    // route.cpp : is the carrier walking now?

// ---------- drawing helpers (main.cpp) ----------
void setColor(float r, float g, float b);
void rect(float x1, float y1, float x2, float y2);
void circle(float cx, float cy, float r);
void halfCircle(float cx, float cy, float r);
void triangle(float x1, float y1, float x2, float y2, float x3, float y3);
void thickLine(float x1, float y1, float x2, float y2, float w);
void drawText(float x, float y, const char* s, void* font = GLUT_BITMAP_HELVETICA_12);
void drawTextCentered(float cx, float y, const char* s, void* font);

// ---------- scene.cpp (Ijma) ----------
void drawBackground();                   // sky, ground, mosque, people, mela ...
void drawForeground();                   // things drawn on top (gift carrier ...)
void sceneKey(unsigned char key);        // graphics keys (N = night ...)
void sceneUpdate();                      // called every frame

// ---------- route.cpp (Arifa) ----------
void routeInit();                        // build the road graph at start
void drawRoadLayer();                    // roads, houses, routes
void drawPanel();                        // right side information panel
void routeKey(unsigned char key);        // algorithm keys (1-5, S, G, A ...)
void routeMouse(int button, float wx, float wy);
void routeUpdate();                      // called every frame

#endif
