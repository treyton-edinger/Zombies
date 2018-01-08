// pine_tree.c	:	renders a simple tree

#include "pine_tree.h"


//#include "grutil/grutil.h"
#include "geom.h"
#include "srVertex.h"
#include <GL/glut.h>


// trunk
static float trunk_color[3] = {0.3, 0.37, 0.1};
static float trunk_height = 3.0;
static float trunk_radius = 0.3;
static int trunk_slices = 16;
static int trunk_stacks = 16;

// cone
static float cone_color[3] = {0.33, 0.51, 0.37};
static float cone_height = 9.0;
static float cone_radius = 1.5;
static int cone_slices = 16;
static int cone_stacks = 16;

void pine_tree_draw() {
	glPushMatrix();
		glRotatef(-90, 1, 0, 0);

		// draw trunk
		glColor3fv(trunk_color);
		srDrawCylinder(trunk_radius*2, trunk_radius*2, trunk_height, trunk_slices, trunk_stacks);

		// draw cone
		glTranslatef(0, 0, trunk_height);
		glColor3fv(cone_color);
		srDrawCylinder(cone_radius*2, 0, cone_height, cone_slices, cone_stacks);

	glPopMatrix();
}

