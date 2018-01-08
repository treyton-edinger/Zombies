// pine_tree.c	:	renders a simple tree

#include "pine_tree.h"
#include "cnv_model.h"

#include "geom.h"

#include <GL/gl.h>
#include <stdlib.h>

static const char *pine_tree_file = "pine_tree.cnv";
static struct cnv_model *pine_tree_model = NULL;

struct pine_tree *pine_tree_construct(const float *pos, float size) {
	struct pine_tree *self = malloc(sizeof(struct pine_tree));
	geom_vector3_copy(pos, self->pos);
	self->size = size;
	return self;
}

void pine_tree_draw(struct pine_tree *self) {

	if (!pine_tree_model) {
		pine_tree_model = cnv_model_construct(pine_tree_file);
		if (!pine_tree_model)
			return;
		printf("loaded %d records from pine_tree model file\n", pine_tree_model->num_records);
	}

	glPushMatrix();
		glTranslatef(self->pos[0], self->pos[1], self->pos[2]);
		float s = self->size * 0.3;
		glScalef(s, s, s);
		cnv_model_draw(pine_tree_model);
	glPopMatrix();
}

void pine_tree_destroy(struct pine_tree *self) {
}

