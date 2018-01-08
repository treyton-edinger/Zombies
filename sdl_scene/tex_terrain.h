#ifndef _tex_terrain_h_
#define _tex_terrain_h_

#include "image_util.h"
#include <GL/gl.h>

struct tex_terrain {
	float size;
	float unit;
	GLuint texture_id;
	struct image *texture_img;
};

struct tex_terrain *tex_terrain_construct();
void tex_terrain_destroy(struct tex_terrain *self);
void tex_terrain_draw(struct tex_terrain *self);

#endif // _tex_terrain_h_
