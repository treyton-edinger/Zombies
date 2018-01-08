// fence.c	:	renders a simple fence around perimeter

#include "road.h"


//#include "grutil/grutil.h"
#include "geom.h"
#include "srVertex.h"
#include <GL/glut.h>


// trunk
static float road_color[3] = {.1, .1, .1};
static float divider_color[3] = {1, 1, 1};

// cone

void road_draw() {

	int save_render = 0;

	glPushMatrix();
		   // draw fence
		glColor3fv(road_color);
      glBegin(GL_POLYGON);
        srVertex3r(7,0.1,-28);
        srVertex3r(-7,0.1,-28);
        srVertex3r(-7,0.1,-100);
        srVertex3r(7,0.1,-100);
        glEnd();

        int i=0;
        int z =-29;
        glColor3fv(divider_color);
        for(i=0;i< 9; i++){
          glBegin(GL_POLYGON);
          srVertex3r(0.5,0.2,z);
          srVertex3r(-0.5,0.2,z);
          srVertex3r(-0.5,0.2,z-4);
          srVertex3r(0.5,0.2,z-4);
          glEnd();
          z=z-8;

        }

	glPopMatrix();
}
