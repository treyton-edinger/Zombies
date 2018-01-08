#include "house.h"
#include "geom.h"
#include "srVertex.h"
#include <GL/glut.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

//This creates 16 primitives. 4 for top. 12 for the cube

static float color[3] = {0,0.3, 0.3};
static float top_color[4][3] = {
	{0.9,0.9,0.9},
	{0.9,0.9,0.9},
	{0,0.2,0.2},
	{0,0.2,0.2}
	};

static float box_side = 25;
static float top_height = 20;

static const char *geom_file = "house.obj";

//Declare functions here
static void draw_box(float side);
static void draw_top(float height, float side);



int file_exists_house(const char *fname){
	struct stat sbuf;
	int rc = stat(fname, &sbuf);
	return rc == 0;
}


void house_draw(){

	int save_render = 0;
	if(!file_exists_house(geom_file)){
		save_render =1;
		srVertex_set_save_file(geom_file);
		srEnable(1);
	}

	glColor3fv(color);

	glPushMatrix();
		draw_box(box_side);
		//glTranslatef(0, box_side,0);

		draw_top(top_height, box_side);
	glPopMatrix();

	if(save_render){
		srEnable(0);
	}
}


static void draw_box(float side){
	glPushMatrix();
		srDrawBox(side, side, side);
	glPopMatrix();
}


//This could be looped over but I had to figure this out manually so not as clean
static void draw_top(float height, float side){
	glPushMatrix();

	float top[] = {0, height, 0};
	float P1[] = {side/2, side/2, side/2};
	float P2[] = {side/2, side/2, -side/2};
	float P3[] = {-side/2, side/2, side/2};
	float P4[] = {-side/2, side/2, -side/2};

	glColor3fv(top_color[0]);
	glBegin(GL_TRIANGLES);
		srVertex3rv(top);
		srVertex3rv(P1);
		srVertex3rv(P2);
	glEnd();

	glColor3fv(top_color[1]);
	glBegin(GL_TRIANGLES);
		srVertex3rv(top);
		srVertex3rv(P4);
		srVertex3rv(P3);
	glEnd();

	glColor3fv(top_color[2]);
	glBegin(GL_TRIANGLES);
		srVertex3rv(top);
		srVertex3rv(P4);
		srVertex3rv(P2);
	glEnd();

	glColor3fv(top_color[3]);
	glBegin(GL_TRIANGLES);
		srVertex3rv(top);
		srVertex3rv(P3);
		srVertex3rv(P1);
	glEnd();




	glPopMatrix();
}
