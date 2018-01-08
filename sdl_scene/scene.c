#include "scene.h"
#include "pine_tree.h"
#include "temple.h"
#include "tex_terrain.h"

#include "geom.h"

#include <GL/gl.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define NUM_TREES 10

// module func decls
static void create_trees(struct scene *scene);
static float randf();

// module func impl
struct scene *scene_construct(void) {
	struct scene *self = malloc(sizeof(struct scene));
	self->trees = malloc(sizeof(struct pine_tree*) * NUM_TREES);
	self->num_trees = NUM_TREES;
	create_trees(self);
	self->terrain = tex_terrain_construct();

	float temple_pos[3] = {0, 7, 0};
	self->temple = temple_construct(temple_pos, 1.0);

	return self;
}

static void create_trees(struct scene *scene) {
	srand(8);
	int i;
	for(i = 0; i < scene->num_trees; ++i) {
		float minx = -100;
		float minz = -100;
		float b = 200;
		float bx = b;
		float bz = b;
		float x = minx + randf()*bx;
		float y = 0;
		float z = minz + randf()*bz;
		float size = 1 + randf() * 3;
		float pos[3] = {x, y, z};
		scene->trees[i] = pine_tree_construct(pos, size);
	}
}

static float randf() {
	return (float)rand()/(float)RAND_MAX;
}

void scene_destroy(struct scene *self) {
	free(self->temple);
	free(self->terrain);
	for(int i = 0; i < self->num_trees; ++i) {
		pine_tree_destroy(self->trees[i]);
	}
	free(self->trees);
	free(self);
}

void scene_draw(struct scene *self) {
	// draw terrain
	tex_terrain_draw(self->terrain);

	// draw trees
	int i;
	for(i = 0; i < self->num_trees; ++i) {
		struct pine_tree *tree = self->trees[i];
		pine_tree_draw(tree);
	}

	// draw temple
	temple_draw(self->temple);
}

