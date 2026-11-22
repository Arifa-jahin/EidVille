// ============================================================
//  main.cpp  -  window, timer and drawing helpers
//  (shared file: both members should change it only rarely)
// ============================================================
#include "common.h"

int frameNo = 0;
int slide = 1;                 // 1 = Chand Raat, 2 = EidVille town, 3 = Eid for everyone
int slideFrame = 0;            // frames since the current slide appeared
int nextSlide = 1;             // slide we are moving to
float slideMove = 0;           // 0..1 progress of the sliding animation
int vpX = 0, vpY = 0, vpW = SCR_W, vpH = SCR_H;   // drawing area inside the window

// ---------------- drawing helpers ----------------
// solid colour (alpha = 1)
void setColor(float r, float g, float b) { glColor4f(r, g, b, 1.0f); }

void rect(float x1, float y1, float x2, float y2) {
    glBegin(GL_QUADS);
    glVertex2f(x1, y1); glVertex2f(x2, y1); glVertex2f(x2, y2); glVertex2f(x1, y2);
    glEnd();
}

void circle(float cx, float cy, float r) {
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx, cy);
    for (int i = 0; i <= 40; i++) {
        float a = 2 * PI * i / 40;
        glVertex2f(cx + r * cos(a), cy + r * sin(a));
    }
    glEnd();
}

void halfCircle(float cx, float cy, float r) {      // upper half
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx, cy);
    for (int i = 0; i <= 30; i++) {
        float a = PI * i / 30;
        glVertex2f(cx + r * cos(a), cy + r * sin(a));
    }
    glEnd();
}

void triangle(float x1, float y1, float x2, float y2, float x3, float y3) {
    glBegin(GL_TRIANGLES);
    glVertex2f(x1, y1); glVertex2f(x2, y2); glVertex2f(x3, y3);
    glEnd();
}

void thickLine(float x1, float y1, float x2, float y2, float w) {
    float dx = x2 - x1, dy = y2 - y1, L = sqrt(dx * dx + dy * dy);
    if (L == 0) return;
    float nx = -dy / L * w / 2, ny = dx / L * w / 2;
    glBegin(GL_QUADS);
    glVertex2f(x1 + nx, y1 + ny); glVertex2f(x2 + nx, y2 + ny);
    glVertex2f(x2 - nx, y2 - ny); glVertex2f(x1 - nx, y1 - ny);
    glEnd();
}

void drawText(float x, float y, const char* s, void* font) {
    glRasterPos2f(x, y);
    for (const char* c = s; *c; c++) glutBitmapCharacter(font, *c);
}

void drawTextCentered(float cx, float y, const char* s, void* font) {
    int w = 0;
    for (const char* c = s; *c; c++) w += glutBitmapWidth(font, *c);
    drawText(cx - w / 2.0f, y, s, font);
}

// ---------------- slides ----------------
void drawTown() {
    drawBackground();     // Ijma
    drawRoadLayer();      // Arifa
    drawForeground();     // Ijma
    drawPanel();          // Arifa
}

void drawSlide(int s) {
    if (s == 1)      drawMoonNight();
    else if (s == 2) drawTown();
    else             drawSharing();
}

// three dots at the bottom show which slide is open
void drawSlideDots() {
    for (int i = 1; i <= 3; i++) {
        glColor4f(0, 0, 0, 0.35f);
        circle(520 + (i - 1) * 30, 14, 8);
        glColor4f(1, 1, 1, i == slide ? 1.0f : 0.45f);
        circle(520 + (i - 1) * 30, 14, i == slide ? 5.5f : 4);
    }
    glColor4f(1, 1, 1, 0.6f);
    drawText(380, 9, "<  Left / Right arrow  >", GLUT_BITMAP_HELVETICA_10);
}

void goToSlide(int s) {
    if (s < 1 || s > 3 || s == slide || slideMove > 0) return;
    nextSlide = s;
    slideMove = 0.001f;               // start the sliding animation
}

// ---------------- GLUT callbacks ----------------
void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();
    if (slideMove > 0) {
        // smooth movement: slow at the start and the end
        float t = slideMove * slideMove * (3 - 2 * slideMove);
        float dir = (nextSlide > slide) ? 1.0f : -1.0f;
        glPushMatrix();
        glTranslatef(-dir * t * WIN_W, 0, 0);           // old slide moves out
        drawSlide(slide);
        glPopMatrix();
        glPushMatrix();
        glTranslatef(dir * (1 - t) * WIN_W, 0, 0);      // new slide comes in
        drawSlide(nextSlide);
        glPopMatrix();
    } else {
        drawSlide(slide);
    }
    drawSlideDots();
    glutSwapBuffers();
}

void keyboard(unsigned char key, int, int) {
    if (key == 27) exit(0);   // ESC
    if (key == 'm' || key == 'M') { sceneKey(key); return; }   // Eid song on / off on every slide
    if (slide == 2) {         // town keys only work on the town slide
        sceneKey(key);
        routeKey(key);
    } else if (key == ' ') {
        slideFrame = 0;       // SPACE: play the slide's story again
    }
}

void specialKey(int key, int, int) {
    if (key == GLUT_KEY_RIGHT) goToSlide(slide + 1);
    if (key == GLUT_KEY_LEFT)  goToSlide(slide - 1);
}

void mouse(int button, int state, int mx, int my) {
    if (state != GLUT_DOWN) return;
    if (button == 3) { goToSlide(slide - 1); return; }   // mouse wheel up
    if (button == 4) { goToSlide(slide + 1); return; }   // mouse wheel down
    if (slide != 2) return;
    float wx = (mx - vpX) * (float)WIN_W / vpW;                              // window pixel
    float wy = (glutGet(GLUT_WINDOW_HEIGHT) - my - vpY) * (float)WIN_H / vpH; // -> world
    routeMouse(button, wx, wy);
}

void timer(int) {
    frameNo++;
    slideFrame++;
    if (slideMove > 0) {
        slideMove += 0.025f;          // about 40 frames for one slide change
        if (slideMove >= 1) { slideMove = 0; slide = nextSlide; slideFrame = 0; }
    }
    sceneUpdate();
    routeUpdate();
    glutPostRedisplay();
    glutTimerFunc(16, timer, 0);   // about 60 frames per second
}

// keep the picture's shape when the window is resized
void reshape(int w, int h) {
    float want = (float)WIN_W / WIN_H;
    if ((float)w / h > want) { vpH = h; vpW = (int)(h * want); }
    else                     { vpW = w; vpH = (int)(w / want); }
    vpX = (w - vpW) / 2;
    vpY = (h - vpH) / 2;
    glViewport(vpX, vpY, vpW, vpH);
}

void init() {
    glClearColor(0, 0, 0, 1);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, WIN_W, 0, WIN_H);
    glMatrixMode(GL_MODELVIEW);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    routeInit();
    sceneKey('m');           // the Eid song starts with the program
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(SCR_W, SCR_H);
    glutInitWindowPosition(60, 20);
    glutCreateWindow("EidVille - Smart Eid Gift Exchange");
    init();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKey);
    glutMouseFunc(mouse);
    glutTimerFunc(16, timer, 0);
    glutMainLoop();
    return 0;
}