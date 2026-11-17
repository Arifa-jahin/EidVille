// ============================================================
//  sharing.cpp  -  Slide 3: Eid for everyone
//  Owner: Arifa
// ============================================================
#include "common.h"

// placeholder until the full slide is ready
void drawSharing() {
    glBegin(GL_QUADS);
    glColor4f(0.40f, 0.25f, 0.20f, 1); glVertex2f(0, 0); glVertex2f(WIN_W, 0);
    glColor4f(0.05f, 0.05f, 0.12f, 1); glVertex2f(WIN_W, WIN_H); glVertex2f(0, WIN_H);
    glEnd();
    setColor(1.0f, 0.85f, 0.35f);
    drawTextCentered(WIN_W / 2, 420, "Slide 3: Eid for everyone", GLUT_BITMAP_TIMES_ROMAN_24);
    setColor(0.85f, 0.85f, 0.95f);
    drawTextCentered(WIN_W / 2, 380, "coming soon...", GLUT_BITMAP_HELVETICA_18);
}
