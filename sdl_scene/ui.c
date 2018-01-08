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
	self->frame_rate_status = status_line_construct("frame_rate", "frame_rate", "", row++);
/*
	// construct edit line
	self->input_line = edit_line_construct("input_line", edit_line_row);
	self->input_line->visible = 0;
*/
	return self;
}

void ui_draw(struct ui *self) {
	status_line_draw(self->frame_rate_status);
//	edit_line_draw(self->input_line);
}

void ui_destroy(struct ui *self) {
//	edit_line_destroy(self->input_line);
	status_line_destroy(self->frame_rate_status);
	free(self);
}

