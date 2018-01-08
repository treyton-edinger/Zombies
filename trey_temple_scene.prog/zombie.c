// temple.c	:	draws temple using srVertex commands
#include "zombie.h"
#include "temple.h"

#include "geom.h"
#include "srVertex.h"
#include "temple.h"

#include <GL/glut.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

// person attributes
float * person_color;// = {{0.67, 0.67, 0.7}, {0.2, 0.2, 1.0}, {1.0, 1.0, 1.0}};
static float body_height = 5.0;
float body_radius;// = 2.5;
float leg_radius;// = 0.8;
float leg_height;// = 5.0;
float arm_radius;// = 0.5;
float arm_height;// = 6.0;
float neck_radius;// = 1.0;
float throat_height = .5;
float head_radius;// = 2.0;
static int person_slices = 30;
static int person_stacks = 30;
int hat_type;

static const char *geom_file = "zombie.cnv";

static void draw_legs(void);
static void draw_body(void);
static void draw_part(float radius, float height, int stacks, int slices);
static void draw_arms(void);
static void draw_neck(void);
static void draw_head(void);

void draw_zomb(float * color) {
	// save geom to file if needed
	int save_render = 0;
	if (!file_exists(geom_file)) {
		save_render = 1;
		srVertex_set_save_file(geom_file);
		srEnable(1);
	}

	//hat_type = hat;
	body_radius = body_height/3.0;
	leg_radius = body_radius/3.0;
	leg_height = body_height * 3.0/4.0;
	arm_radius = leg_radius * 2.0/3.0;
	arm_height = body_height * 4.0/5.0;
	head_radius = body_radius * 3.0/4.0;
	neck_radius = head_radius/2.0;
	person_color = color;
	glColor3fv(person_color);
	draw_legs();
	draw_body();
	draw_arms();
	draw_neck();
	draw_head();

	// finish geometry save
	if (save_render) {
		srEnable(0);
	}
}

static void draw_part(float radius, float height, int slices, int stacks) {
	glPushMatrix();
		glRotatef(-90, 1, 0, 0);
			srDrawCylinder(radius, radius, height, slices, stacks);
	glPopMatrix();
}

static void draw_legs() {
	// determine initial starting point from origin for first leg
	float dx = body_radius - leg_radius;
	// distance between legs
	float leg_unit = dx * 2;

	// translate left to draw first leg and then translate right to draw second
	int i;
	glPushMatrix();
		glTranslatef(-dx, 0, 0);
		for(i = 0; i < 2; ++i) {
			draw_part(leg_radius, leg_height, person_slices, person_stacks);
			glTranslatef(leg_unit, 0, 0);
		}
	glPopMatrix();
}

static void draw_body() {
	GLUquadric* origin = gluNewQuadric();
	float dy = leg_height;

	glPushMatrix();
		glTranslatef(0, dy, 0);
		draw_part(body_radius, body_height, person_slices, person_stacks);
	glPopMatrix();

	glPushMatrix();
		glTranslatef(0, dy + body_height, 0);
		glRotatef(90, 1.0, 0, 0);
		gluDisk(origin, 0, body_radius, person_slices, person_stacks);
	glPopMatrix();
	gluDeleteQuadric(origin);
}

static void draw_arms() {
	// find distance between arms and body
	float arm_spacing = body_radius/10.0;
	// calculate initial starting points for arm draw
	float dy = leg_height + (body_height - arm_height);
	float dx = arm_spacing + arm_radius + body_radius;
	// determine distance between each arm
	float arm_unit = (body_radius + arm_radius + arm_spacing) * 2;
	GLUquadric* origin = gluNewQuadric();

	// translate matrix to the left and up to start arm draw and then translate
	// +x to draw second arm
	glPushMatrix();
		glTranslatef(-dx, dy, 0);
		glPushMatrix();
			glTranslatef(0, arm_height - arm_radius, 0);
			glRotatef(-90, 0, 0, 1.0);
			draw_part(arm_radius, arm_spacing + arm_radius * 2, person_slices, person_stacks);
			glTranslatef(0, (arm_spacing + arm_radius * 2) * 2 + body_radius, 0);
			draw_part(arm_radius, arm_spacing + arm_radius * 2, person_slices, person_stacks);
			glTranslatef(arm_height - arm_radius, arm_spacing + arm_radius*2, 0);
			/*if (hat_type == 0) {
				draw_broom(arm_radius/2.0);
				glColor3fv(person_color);
			}*/
		glPopMatrix();
		glTranslatef(0,arm_height,-arm_radius);
		glRotatef(90,1,0,0);

		for(int i = 0; i < 2; ++i) {

			draw_part(arm_radius, arm_height, person_slices, person_stacks);
			glPushMatrix();
				glTranslatef(0, arm_height, 0);
				glRotatef(90, 1.0, 0, 0);
				gluDisk(origin, 0, arm_radius, person_slices, person_stacks);
			glPopMatrix();
			glTranslatef(arm_unit, 0, 0);
		}
	glPopMatrix();
	gluDeleteQuadric(origin);
}

static void draw_neck() {
	// determine initial point for neck based on leg and body heights
	float dy = leg_height + body_height;

	// translate matrix by calculated height and draw cyclinder
	glPushMatrix();
		glTranslatef(0, dy, 0);
		draw_part(neck_radius, throat_height, person_slices, person_stacks);
	glPopMatrix();
}
//
static void draw_head() {
	// create new quardic for sphere
	GLUquadric* origin = gluNewQuadric();
	// determine initial point to draw head
	float dy = body_height + leg_height + throat_height/2 + head_radius;

	// translate up and draw sphere for head
	glPushMatrix();
		glTranslatef(0, dy, 0);
		gluSphere(origin, head_radius, person_stacks, person_slices);
		glTranslatef(0, head_radius/2, 0);
		/*if (hat_type == 1) {
			draw_tophat(head_radius);
		}
		else {
			draw_witchhat(head_radius);
		}*/
	glPopMatrix();
	gluDeleteQuadric(origin);
}
