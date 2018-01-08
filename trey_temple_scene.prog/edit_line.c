#include "edit_line.h"

#include <GL/glut.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define KEY_EXIT 27
#define KEY_ENTER 13
#define KEY_BS 8

#define MAX_INPUT 256

static float edit_line_color[] = {0.75, 0.75, 0.75};

static void draw_string(const char *s);

struct edit_line *edit_line_construct(const char *name, int row) {
	struct edit_line *self = malloc(sizeof(struct edit_line));
	self->name = strdup(name);
	self->prompt = NULL;
	self->val = NULL;
	self->row = row;
	self->visible = 0;
	self->handler = NULL;	
	return self;
}

void edit_line_destroy(struct edit_line *self) {
	free(self->name);
	free(self->prompt);
	free(self->val);
	free(self);
}

void edit_line_prompt(
	struct edit_line *self, 
	const char *prompt, 
	const char *default_val, 
	const char* (*handler)(const char*)) {

	free(self->prompt);
	self->prompt = strdup(prompt);

	free(self->val);
	self->val = malloc(MAX_INPUT + 1);
	strncpy(self->val, default_val, MAX_INPUT);
	self->val[MAX_INPUT - 1] = '\0';

	self->visible = 1;
	self->handler = handler;
}

int edit_line_keyboard(struct edit_line *self, int key) {

	if (!self->visible)
		return 0;

	int len = strlen(self->val);

	switch (key) {
	case KEY_EXIT:
		self->visible = 0;
		self->handler = NULL;
		break;
	case KEY_ENTER:
		self->visible = 0;
		const char *(*cb)(const char*) = self->handler;
		self->handler = NULL;
		(*cb)(self->val);
		break;
	case KEY_BS:
		if (len > 0) {
			char *pc = self->val + len - 1;
			*pc = '\0';
		}
		break;
	default:
		if (isprint(key) && len < MAX_INPUT) {
			char *pc = self->val + len;
			*pc++ = (char) key;
			*pc = '\0';
		}
		break;
	}
	return 1;
}

void edit_line_draw(struct edit_line *self) {
	if (!self->visible)
		return;

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
	glColor3fv(edit_line_color);

	// set raster pos
	float x0 = -0.99;
	float y0 = 0.95;
	float dy = -0.05;
	float y = y0 + self->row * dy;
	glRasterPos2f(x0,  y);

	// draw
	int len = strlen(self->prompt) + strlen(self->val) + 4;
	char *line = malloc(len);
	sprintf(line, "%s: %s_", self->prompt, self->val);
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

static void draw_string(const char *s) {
	int len = strlen(s);
	for (int i = 0; i < len; ++i) {
		glutBitmapCharacter(GLUT_BITMAP_8_BY_13, s[i]);
	}
}

