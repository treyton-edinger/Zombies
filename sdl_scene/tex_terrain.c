#include "tex_terrain.h"
#include "image_util.h"

#include <GL/gl.h>
#include <GL/glu.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// module function decls
static void init_texture(const char *img_file, struct image *img, GLuint *tex_id);

// module data
static float default_size = 500;
static float default_unit = 10;
static float terrain_color[3] = {0.3, 0.2, 0};
static const char *texture_img_file = "green-grass-1024.jpg";

// module function defns
struct tex_terrain *tex_terrain_construct() {
	struct tex_terrain *self = malloc(sizeof(struct tex_terrain));
	self->size = default_size;
	self->unit = default_unit;

	// create texture
	self->texture_img = malloc(sizeof(struct image));
	init_texture(texture_img_file, self->texture_img, &self->texture_id);

	return self;
}

void tex_terrain_destroy(struct tex_terrain *self) {
	free(self);
}

void tex_terrain_draw(struct tex_terrain *self) {
	glColor3fv(terrain_color);

	// set color to white for brightest texture
	glColor3f(1, 1, 1);

	int wireframe = 0;
	int draw_op = wireframe ? GL_LINE_LOOP : GL_QUADS;
	float tsize = self->size;
	float tinc = self->unit;
	float zmin = -tsize;
	float zmax = tsize;
	float xmin = -tsize;
	float xmax = tsize;

	// set texture
	glBindTexture(GL_TEXTURE_2D, self->texture_id);
	glNormal3f(0, 1, 0);
	float x, z;
	for (z = zmin; z < zmax; z += tinc) {
		for (x = xmin; x < xmax; x += tinc) {
			glBegin(draw_op);

			glTexCoord2f(0, 1);
			glVertex3f(x, 0, z);

			glTexCoord2f(0, 0);
			glVertex3f(x, 0, z + tinc);

			glTexCoord2f(1, 0);
			glVertex3f(x + tinc, 0, z + tinc);

			glTexCoord2f(1, 1);
			glVertex3f(x + tinc, 0, z);

			glEnd();
		}
	}

	// unbind texture after use
	glBindTexture(GL_TEXTURE_2D, 0);
}


static void init_texture(const char *img_file, struct image *img, GLuint *tex_id) {
	// load image
	memset(img, 0, sizeof(struct image));
	int rc = image_load(img_file, img);
	if(rc != IMAGEUTIL_NOERROR) {
		printf("failed loading img: %s\n", img_file);
		return;
	}

	// create texture object
	glGenTextures(1, tex_id);

	// bind tex object to current state
	glBindTexture(GL_TEXTURE_2D, *tex_id);

	// define tex image
	glTexImage2D(GL_TEXTURE_2D,
		0,
		GL_RGB,
		img->width,
		img->height,
		0,
		img->gl_format,
		GL_UNSIGNED_BYTE,
		img->pixels);
	
	// set params
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); 
}

