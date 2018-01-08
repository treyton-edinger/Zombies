#include "ui.h"

#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int edit_line_row = 36;

char line_buf[256];	// intermediate buffer for status and edit lines

struct ui *ui_construct() {
	struct ui *self = malloc(sizeof(struct ui));
	
	// construct status lines
	int row = 1;
	self->mouse_pos_status = status_line_construct("mouse_pos", "mouse_pos", "", row++);
/*
	self->lookat_status = status_line_construct("lookat", "lookat", "1", row++);
	self->mouse_status = status_line_construct("mouse", "mouse", "", row++);
	self->transform_status = status_line_construct("transform", "transform", "", row++);
	self->x_axis_rotation_status = status_line_construct("x_rotation", "x rotation", "0", row++);
	self->y_axis_rotation_status = status_line_construct("y_rotation", "y rotation", "0", row++);
	self->translation_status = status_line_construct("translation", "translation", "( 0.0, 0.0, 0.0 )", row++);

	for (int i = 0; i < 4; ++i) {
		char name[32];
		sprintf(name, "mm%d", i);
		self->mm_status[i] = status_line_construct(name, name, "", row++);
	}
*/
	// construct edit line
	self->input_line = edit_line_construct("input_line", edit_line_row);
	self->input_line->visible = 0;

	return self;
}

void ui_draw(struct ui *self) {
	status_line_draw(self->mouse_pos_status);
/*
	status_line_draw(self->x_axis_rotation_status);
	status_line_draw(self->y_axis_rotation_status);
	status_line_draw(self->lookat_status);
	status_line_draw(self->mouse_status);
	status_line_draw(self->transform_status);
	status_line_draw(self->translation_status);

	for (int i = 0; i < 4; ++i)
		status_line_draw(self->mm_status[i]);
*/
	edit_line_draw(self->input_line);
}

void ui_destroy(struct ui *self) {
	edit_line_destroy(self->input_line);
/*
	for (int i = 0; i < 4; ++i)
		status_line_destroy(self->mm_status[i]);

	status_line_destroy(self->translation_status);
	status_line_destroy(self->lookat_status);
	status_line_destroy(self->y_axis_rotation_status);
*/
	status_line_destroy(self->mouse_pos_status);
	free(self);
}

