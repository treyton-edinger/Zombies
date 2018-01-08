#include "status_line.h"

#include "geom.h"

#include <FTGL/ftgl.h>

#include <GL/glut.h>
#include <stdio.h>
#include <string.h>

// fwd decls
static int init_font(void);
static void draw_string(const char *s);

// module data
static float default_status_line_color[] = { 1.0, 1.0, 1.0 };
static FTGLfont *status_font = NULL;

struct status_line *status_line_construct(
	const char *name, 
	const char *label, 
	const char *val, 
	int row) {
	struct status_line *self = malloc(sizeof(struct status_line));
	self->name = strdup(name);
	self->label = strdup(label);
	self->val = strdup(val);
	self->row = row;
	geom_vector3_copy(default_status_line_color, self->color);
	return self;
}

void status_line_destroy(struct status_line *self) {
	free(self->name);
	free(self->label);
	free(self->val);
	free(self);
}

void status_line_set(struct status_line *self, const char* val) {
	free(self->val);
	self->val = strdup(val);
}

void status_line_draw(struct status_line *self) {
	// save current matrices
	glPushMatrix();
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();

	// set identity matrices
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// save raster state
	glPushAttrib(GL_CURRENT_BIT | GL_LIGHTING_BIT);

	glDisable(GL_LIGHTING);

	// set raster pos
	float x0 = -0.99;
	float y0 = 0.95;
	float dy = -0.05;
	float y = y0 + self->row * dy;
	glColor3fv(self->color);
	glRasterPos2f(x0,  y);

	// draw
	int len = strlen(self->label) + strlen(self->val) + 3;
	char *line = malloc(len);
	sprintf(line, "%s: %s", self->label, self->val);

	draw_string(line);
	free(line);

	// restore raster state
	glPopAttrib();

	// restore matrices
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

static int init_font(void) {
	if (status_font == NULL) {
		// Create a pixmap font from a TrueType file. 
		status_font = ftglCreatePixmapFont("FreeMono.ttf");

		// check font
		if (!status_font) {
			fprintf(stderr, "ftglCreatePixmapFont() failed\n");
		}
	
		// Set the font size
		ftglSetFontFaceSize(status_font, 16, 16);
	}

	return status_font != NULL;
}

static void draw_string(const char *text) {
	if (!init_font())
		return;
	ftglRenderFont(status_font, text, FTGL_RENDER_ALL);
}
/*

static void draw_string(const char *s) {
	int len = strlen(s);
	for (int i = 0; i < len; ++i) {
//		glutBitmapCharacter(GLUT_BITMAP_8_BY_13, s[i]);
	}
}
*/

void status_line_cleanup(void) {
	// Destroy the font object. 
	ftglDestroyFont(status_font);
}

