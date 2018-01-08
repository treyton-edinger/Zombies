// pine_tree.c	:	renders a guard tower

#include "guard_tower.h"


//#include "grutil/grutil.h"
#include "geom.h"
#include "srVertex.h"
#include <GL/glut.h>


// trunk
static float base_color[3] = {0.21, 0.21, 0.21};
static float base_height = 30.0;
static float base_radius = 0.8;
static int base_slices = 16;
static int base_stacks = 16;


// cone
static float box_color[3] = {.9, .9, .9};
static float box_height = 9.0;

void guard_tower_draw() {

	



	glPushMatrix();
		glRotatef(-90, 1, 0, 0);

		// draw trunk
		glColor3fv(base_color);
		srDrawCylinder(base_radius*2, base_radius*2, base_height, base_slices, base_stacks);

		// draw cone
		glTranslatef(0, 0, base_height);
		glColor3fv(box_color);
    glRotatef(90,1,0,0);
  int i=0;
  int rot=-90;
    for(i=0;i<4;i++){

		glBegin(GL_LINE_LOOP);
      srVertex3r(6,6,6);
      srVertex3r(-6,6,6);
      srVertex3r(-6,0,6);
      srVertex3r(6,0,6);
    glEnd();
    glRotatef(rot,0,1,0);
    rot-=90;

    glBegin(GL_POLYGON);
    srVertex3r(-6,0,6);
    srVertex3r(-6,0,-6);
    srVertex3r(6,0,-6);
    srVertex3r(6,0,6);
    glEnd();
}


	glPopMatrix();
}
