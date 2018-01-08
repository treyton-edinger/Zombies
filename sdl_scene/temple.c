// temple.c	:	renders a simple tree

#include "temple.h"
#include "cnv_model.h"

#include "geom.h"

#include <GL/gl.h>
#include <stdlib.h>

static const char *temple_file = "temple.cnv";
static struct cnv_model *temple_model = NULL;
static float temple_color[3] = {0.8, 0.8, 0.8};

struct temple *temple_construct(const float *pos, float size) {
	struct temple *self = malloc(sizeof(struct temple));
	geom_vector3_copy(pos, self->pos);
	self->size = size;
	return self;
}

void temple_draw(struct temple *self) {

	if (!temple_model) {
		temple_model = cnv_model_construct(temple_file);
		if (!temple_model)
			return;
		printf("loaded %d records from temple model file\n", temple_model->num_records);
	}

	glPushMatrix();
		glTranslatef(self->pos[0], self->pos[1], self->pos[2]);
		float s = self->size;
		glScalef(s, s, s);
		glColor3fv(temple_color);
		cnv_model_draw(temple_model);
	glPopMatrix();
}

void temple_destroy(struct temple *self) {
}

