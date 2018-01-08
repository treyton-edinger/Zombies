// temple.c	:	draws temple using srVertex commands

#include "temple.h"

#include "geom.h"
#include "srVertex.h"

#include <GL/glut.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

// temple
static float temple_color[3] = {0.67, 0.67, 0.7};
static float pillar_radius = 0.5;
static float pillar_spacing = 2.0;
static float pillar_height = 10.0;
static int pillar_slices = 16;
static int pillar_stacks = 16;
//static float platform_width = 7.5;
static float platform_height = 0.64;
//static float platform_depth = 12.5 + 2.0;		// pillar depth:(width of 6 pillars + 5 pillar-spacings) + pillar spacing on each side
static float platform_width_inc = 1.0;
static float platform_depth_inc = 1.0;
static float roof_base_height = 0.75;
static float roof_height = 3.0;

// save geometry to file
static const char *geom_file = "temple.obj";

static void draw_platforms(void);
static void draw_pillar(void);
static void draw_pillars(void);
static void draw_roof(void);

int file_exists(const char *fname) {
	struct stat sbuf;
	int rc = stat(fname, &sbuf);
	return rc == 0;
}

void temple_draw() {
	// save geom to file if needed
	int save_render = 0;
	if (!file_exists(geom_file)) {
		save_render = 1;
		srVertex_set_save_file(geom_file);
		srEnable(1);
	}	

	glColor3fv(temple_color);
	draw_platforms();
	draw_pillars();
	draw_roof();

	// finish geometry save 
	if (save_render) {
		srEnable(0);
	}
}

static void draw_roof(void) {
	float pillar_unit = pillar_spacing + 2*pillar_radius;
	float roof_width = pillar_unit*3.6;
	float roof_depth = pillar_unit*4.8;

	glPushMatrix();
		glTranslatef(0, pillar_height/2.0 + roof_base_height/2.0, 0);

		// roof base
		float w = roof_width;
		float h = roof_base_height;
		float d = roof_depth;
		srDrawBox(w, h, d);

		glTranslatef(0, h/2.0, 0);

		srBegin(GL_TRIANGLES);
			// front tri
			srNormal3r(0, 0, 1);
			srVertex3r(-w/2, 0, d/2);
			srVertex3r( w/2, 0, d/2);
			srVertex3r( 0, roof_height, d/2);

			// back tri
			srNormal3r(0, 0, -1);
			srVertex3r(-w/2, 0, -d/2);
			srVertex3r( 0, roof_height, -d/2);
			srVertex3r( w/2, 0, -d/2);

			// left side
			srComment("begin roof left side");
			float lt0[3*3] = {
				-w/2, 0, -d/2,
				-w/2, 0,  d/2,
				0, roof_height, d/2
			};
			float ln[3];
			geom_calc_normal(lt0+0, lt0+3, lt0+6, ln);
			srNormal3rv(ln);
			srVertex3rv(lt0);
			srVertex3rv(lt0+3);
			srVertex3rv(lt0+6);

			float lt1[3*3] = {
				-w/2, 0, -d/2,
				0, roof_height, d/2,
				0, roof_height, -d/2,
			};
			srVertex3rv(lt1);
			srVertex3rv(lt1+3);
			srVertex3rv(lt1+6);
			srComment("end roof left side");

			// right side
			srComment("begin roof right side");
			float rt0[3*3] = {
				 w/2, 0, -d/2,
				 0, roof_height, -d/2,
				 0, roof_height,  d/2,
			};
			float rn[3];
			geom_calc_normal(rt0+0, rt0+3, rt0+6, rn);
			srNormal3rv(rn);
			srVertex3rv(rt0);
			srVertex3rv(rt0+3);
			srVertex3rv(rt0+6);

			float rt1[3*3] = {
				w/2, 0, -d/2,
				0, roof_height, d/2,
				w/2, 0, d/2,
			};
			srVertex3rv(rt1);
			srVertex3rv(rt1+3);
			srVertex3rv(rt1+6);
			srComment("end roof right side");
		srEnd();
	glPopMatrix();
}

static void draw_pillar() {
	glPushMatrix();
		glRotatef(-90, 1, 0, 0);
			srDrawCylinder(pillar_radius, pillar_radius, pillar_height, pillar_slices, pillar_stacks);
	glPopMatrix();
}

static void draw_pillars() {
	// translate to back-left corner (-x/2, 0, -z/2), draw 4 cylinders in x dir
	// translate to front-left corner (-x/2, 0, -z/2), draw 4 cylinders in x dir
	// translate to back-left corner + 1 pillar-spacing in z-dir, draw 4 cylinders in z dir
	// translate to back-right corner + 1 pillar-spacing in z-dir, draw 4 cylinders in z dir
	float dz = 2.5 * pillar_spacing + 2.5 * pillar_radius;
	float dy = pillar_height / 2.0;
	float pillar_unit = pillar_spacing + 2*pillar_radius;
	float w = 3 * pillar_unit;
	float dx = w/2;

	// translate to left-lower-back corner (-x/2, 0, -z/2), draw 4 cylinders in x dir
	int i;
	glPushMatrix();
		glTranslatef(-dx, -dy, -dz);
		for(i = 0; i < 4; ++i) {
			draw_pillar();
			glTranslatef(pillar_unit, 0, 0);
		}
	glPopMatrix();

	// translate to left-lower-front corner (-x/2, 0, -z/2), draw 4 cylinders in x dir
	glPushMatrix();
		glTranslatef(-dx, -dy, dz);
		for(i = 0; i < 4; ++i) {
			draw_pillar();
			glTranslatef(pillar_unit, 0, 0);
		}
	glPopMatrix();

	// translate to left-lower-back corner (-x/2, 0, -z/2), draw 4 cylinders in z dir
	glPushMatrix();
		glTranslatef(-dx, -dy, -dz + pillar_unit);
		for(i = 0; i < 3; ++i) {
			draw_pillar();
			glTranslatef(0, 0, pillar_unit);
		}
	glPopMatrix();

	// translate to right-lower-back corner (-x/2, 0, -z/2), draw 4 cylinders in z dir
	glPushMatrix();
		glTranslatef( dx, -dy, -dz + pillar_unit);
		for(i = 0; i < 3; ++i) {
			draw_pillar();
			glTranslatef(0, 0, pillar_unit);
		}
	glPopMatrix();
}

static void draw_platform(float w, float h, float d) {
	srDrawBox(w, h, d);
}

static void draw_platforms(void) {
	// first, translate down pillar half-height + platform height
	// then, for each platform, 
	//		draw platform
	//		translate down platform height

	float pillar_unit = pillar_spacing + 2*pillar_radius;
	float platform_width = pillar_unit*4;
	float platform_depth = pillar_unit*5;

	float dy = (pillar_height + platform_height) / 2.0;
	glPushMatrix();
		glTranslatef(0, -dy, 0);
		int i;
		for(i = 0; i < 3; ++i) {
			float dw = i * platform_width_inc;
			float dd = i * platform_depth_inc;
			float w = platform_width + dw;
			float h = platform_height;
			float d = platform_depth + dd;
			draw_platform(w, h, d);
			glTranslatef(0, -platform_height, 0);
		}
	glPopMatrix();
}


