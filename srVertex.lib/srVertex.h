#ifndef _srVertex_h_
#define _srVertex_h_

//#include "precision/precision.h"

#include <GL/glut.h>

#define SR_NO_ERROR 0
#define SR_INVALID_VALUE 1
#define SR_FILENAME_TOO_LONG 2

// returns filename that will receive rendering output
const char* srVertex_get_save_file();

// sets filename that will receive rendering output
void srVertex_set_save_file(const char* fname);

// returns and clears error code
int srGetError();

// enables saving of render commands to output file
void srEnable(int enable);

// returns true if saving render commands to output file
int srEnabled();

// appends a comment line to render output
void srComment(const char* fmt, ...);

// issues a glXXX() command
void srColor3r(float x, float y, float z);
void srColor3rv(const float* norm);
void srVertex3r(float x, float y, float z);
void srNormal3r(float x, float y, float z);
void srNormal3rv(const float* norm);
void srVertex3r(float x, float y, float z);
void srVertex3rv(const float* vert);
void srBegin(GLenum glenum);
void srEnd();

// renders a quad using 2 triangles
void srDrawQuad(float* verts);

// renders a box with triangles
void srDrawBox(float width, float height, float depth);

// renders a cylinder with triangles
void srDrawCylinder(float base_radius, float top_radius, float height, int slices, int stacks);

#endif
