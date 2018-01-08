#ifndef _scene_h_
#define _scene_h_

#include "pine_tree.h"
#include "temple.h"
#include "tex_terrain.h"

struct scene {
	struct tex_terrain *terrain;
	struct pine_tree **trees;
	struct temple *temple;
	int num_trees;
};

struct scene *scene_construct(void);
void scene_destroy(struct scene *self);
void scene_draw(struct scene *self);

#endif // _scene_h_
