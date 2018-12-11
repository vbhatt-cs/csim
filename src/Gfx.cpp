#include <array>
#include <GL/glut.h>
#include "Gfx.h"
#include "Global.h"
#include "World.h"

using namespace std;

void Gfx::onFrame()
{
  attacks.clear();
  killed.clear();
}

// save attack for rendering it later
void Gfx::onAttack(const Unit &from, const Unit &to)
{
  attacks.push_back(from);
  attacks.push_back(to);
}

// save kill for rendering it later
// todo: implement
void Gfx::onKill(const Unit &dead)
{
  killed.push_back(dead);
}

void Gfx::onAttacksDone()
{
  // draw world
  // draw effects

  onDisplay();
}

void Gfx::onDisplay()
{
  Timer start;
  
  glClear(GL_COLOR_BUFFER_BIT);
  glPushAttrib(GL_ALL_ATTRIB_BITS);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, world->getWidth(), world->getHeight(), 0, 0, 1);
  glMatrixMode(GL_MODELVIEW);
  // (0,0) top left, (width,height) bottom right

  //glutPostRedisplay();

#if 0
  // render walls
  glBegin(GL_LINE_STRIP);
  glColor3f(1, 1, 1); 
  glVertex2f(1, 1);
  glVertex2f(world->width-1, 1);
  glVertex2f(world->width-1, world->height-1);
  glVertex2f(1, world->height-1);
  glVertex2f(1, 1);
  glEnd();
#endif

  // render units

  // precompute circle data

  array<double, 17> cosTab, sinTab;

  for (int j=0; j <= 16; ++j) {
    const double angle((j%16)*M_PI*2/16);
    cosTab[j] = cos(angle);
    sinTab[j] = sin(angle);
  }
  
  for (auto &p : world->getUnits()) {

    const Unit &u = p.second;
    // cout << "u " << u.pos.x << " " << u.pos.y << " " << u.radius << endl;

    if (u.owner == 0) {
      glColor3f(1, 0.4f, 0);
    } else {
      glColor3f(0, 1, 1);
    }

    // approximate circle

    glBegin(GL_LINE_STRIP);
    for (int j=0; j <= 16; ++j) {
      glVertex2f((float)(u.pos.x + cosTab[j] * u.radius),
                 (float)(u.pos.y + sinTab[j] * u.radius));
      // cout << pos.x + cos(angle) * radius << " " << pos.y + sin(angle)*radius << endl;
    }
    glEnd();

    // hp circle
    
    glBegin(GL_LINE_STRIP);
    double r = u.radius * 0.8 * u.hp / (double) u.maxHp;
    for (int j=0; j <= 16; ++j) {
      glVertex2f((float)(u.pos.x + cosTab[j] * r), 
                 (float)(u.pos.y + sinTab[j] * r));
      // cout << pos.x + cos(angle) * radius << " " << pos.y + sin(angle)*radius << endl;
    }
    glEnd();

#if 0    
    // print the hp inside the unit
		
    glRasterPos2f(u.pos.x-8, u.pos.y+3);
    stringstream hpsStream;
    hpsStream << u.hp;
    const string temp = hpsStream.str();
    const char *hps = temp.c_str();
    int hpsLen = (int)strlen(hps);

    for (int i = 0; i < hpsLen; i++) {
      glutBitmapCharacter(GLUT_BITMAP_8_BY_13, hps[i]);
    }
#endif
    
  }

  // render attacks
  // todo: draw arrow heads to indicate who is attacking whom
  size_t j=0;
  assert((attacks.size() & 1) == 0);

  while (j < attacks.size()) {
    
    glBegin(GL_LINE_STRIP);
    glColor3f(1, 1, 1);
    const Unit &u = attacks[j];
    const Unit &v = attacks[j+1];
    glVertex2f(u.pos.x, u.pos.y);
    glVertex2f(v.pos.x, v.pos.y);
    glEnd();
    //cout << "plot: " << sqrt(world->distance2(u, v)) << " " << u.attack_radius + v.radius << endl;
    j += 2;
  }

  glPopAttrib();
  glutSwapBuffers();

  Timer end;

  displayStats.update(end.diff(start));
}

void Gfx::onGameEnd()
{
}
