// fence.c	:	renders a simple fence around perimeter

#include "fence.h"

//#include "grutil/grutil.h"
#include "geom.h"
#include "srVertex.h"
#include <GL/glut.h>


// trunk
static float fence_color[3] = {1, 0.64, 0};
static float fence_height = 8.0;


// cone

void fence_draw() {
	
	glPushMatrix();
		   // draw fence
		glColor3fv(fence_color);
      glBegin(GL_POLYGON);
        srVertex3r(-28,0,28);
        srVertex3r(-28,fence_height,28);
        srVertex3r(28,fence_height,28);
        srVertex3r(28,0,28);
        glEnd();



	glPopMatrix();


}
