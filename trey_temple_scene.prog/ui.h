#ifndef _ui_h_
#define _ui_h_

#include "edit_line.h"
#include "status_line.h"

struct ui {
	struct status_line *mouse_pos_status;
/*
	struct status_line *lookat_status;		// toggles application of gluLookAt()
	struct status_line *mouse_status;		// toggles mouse rotate x or y
	struct status_line *transform_status;		// toggles transform done by opengl or application
	struct status_line *x_axis_rotation_status;	// shows x axis rotation
	struct status_line *y_axis_rotation_status;	// shows y axis rotation
	struct status_line *translation_status;

	struct status_line *mm_status[4];
*/
	struct edit_line *input_line;
};

struct ui *ui_construct();
void ui_destroy(struct ui *self);

void ui_draw(struct ui *self);

#endif
