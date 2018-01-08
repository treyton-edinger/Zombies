// srVertex.c	:	implements parallel rendering and saving of render commands to an output (.obj) file

#include "srVertex.h"

//#include "grutil/grutil.h"
#include "geom.h"

#include <GL/glut.h>

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>

#define CACHE_SIZE	240

static int save = 0;
static char save_file[FILENAME_MAX] = "srVertex.log";
static FILE* fp = NULL;
static float mvm[16];		// holds modelview matrix
static int srErrorVal = SR_NO_ERROR;

static void srError(int err) {
	srComment("error %d", err);
	srErrorVal = err;
}

int srGetError() {
	int rc = srErrorVal;
	srErrorVal = SR_NO_ERROR;
	return rc;
}

const char* srVertex_get_save_file() {
	return save_file;
}

void srVertex_set_save_file(const char* fname) {
	if(strlen(fname) >= sizeof(save_file)) {
		srError(SR_FILENAME_TOO_LONG);
		return;
	}
	strcpy(save_file, fname);
}

static const char* srGLenum_tostring(GLenum glenum) {
	switch(glenum) {
	case GL_TRIANGLES:
		return "triangles";
	default:
		return "unknown_enum";
	}
}

int srEnabled(){return save;}

void srEnable(int enable) {
	if(enable) {
		fp = fopen(save_file, "a");
	}
	else {
		fclose(fp);
		fp = NULL;
	}
	save = enable;
}

void srComment(const char* fmt, ...) {
	if(!fp)
		return;
	fprintf(fp, "# ");

	va_list va;
	va_start(va, fmt);
	vfprintf(fp, fmt, va);
	va_end(va);

	fprintf(fp, "\n");
}

static void getTransformedVector(const float* vec3, float* vec3ret) {
	float vert4[4] = {vec3[0], vec3[1], vec3[2], 0};
	float tvert4[3];
	float mvmt[16];
	geom_matrix4_transposed(mvm, mvmt);
	geom_matrix4_vector4_mul(mvmt, vert4, tvert4);
	geom_vector3_set(vec3ret, tvert4[0], tvert4[1], tvert4[2]);
}

static void getTransformedPoint(const float* vec3, float* vec3ret) {
	float vert4[4] = {vec3[0], vec3[1], vec3[2], 1};
	float tvert4[3];
	float mvmt[16];
	geom_matrix4_transposed(mvm, mvmt);
	geom_matrix4_vector4_mul(mvmt, vert4, tvert4);
	geom_vector3_set(vec3ret, tvert4[0], tvert4[1], tvert4[2]);
}

void srColor3r(float x, float y, float z) {
	if(save) {
		float color[3] = {x, y, z};
//		float tnorm[3];
//		getTransformedVector(norm, tnorm);
		fprintf(fp, "c %f %f %f\n", (float)color[0], (float)color[1], (float)color[2]);
	}
	glColor3f(x, y, z);
}

void srColor3rv(const float* color) {
	srColor3r(color[0], color[1], color[2]);
}

void srNormal3r(float x, float y, float z) {
	if(save) {
		float norm[3] = {x, y, z};
		float tnorm[3];
		getTransformedVector(norm, tnorm);
		fprintf(fp, "n %f %f %f\n", (float)tnorm[0], (float)tnorm[1], (float)tnorm[2]);
	}
	glNormal3f(x, y, z);
}

void srNormal3rv(const float* norm) {
	srNormal3r(norm[0], norm[1], norm[2]);
}

void srVertex3r(float x, float y, float z) {
	// save transformed vert
	if(save) {
		float vert[3] = {x, y, z};
		float tvert[3];
		getTransformedPoint(vert, tvert);
		fprintf(fp, "v %f %f %f\n", (float)tvert[0], (float)tvert[1], (float)tvert[2]);
	}
//	grutil_inc_verts_drawn(1);
	glVertex3f(x, y, z);
}

void srVertex3rv(const float* vert) {
	srVertex3r(vert[0], vert[1], vert[2]);
}

void srBegin(GLenum glenum) {

	if(save) {
		// get mvm for vertex saves
		glGetFloatv(GL_MODELVIEW_MATRIX, mvm);

		fprintf(fp, "# begin %s\n", srGLenum_tostring(glenum));
	}
	glBegin(glenum);
}

void srEnd() {
	if(save)
		fprintf(fp, "# end\n");
	glEnd();
}

void srDrawQuad(float* verts) {
	srComment("begin quad");
	srBegin(GL_TRIANGLES);
		srVertex3rv(verts+3*0);
		srVertex3rv(verts+3*1);
		srVertex3rv(verts+3*2);

		srVertex3rv(verts+3*0);
		srVertex3rv(verts+3*2);
		srVertex3rv(verts+3*3);
	srEnd();
	srComment("end quad");
}

void srDrawBox(float width, float height, float depth) {
	float w = width/2;
	float h = height/2;
	float d = depth/2;
	float verts[8*3] = {
		-w, -h, -d,
		 w, -h, -d,
		-w,  h, -d,
		 w,  h, -d,
		-w, -h,  d,
		 w, -h,  d,
		-w,  h,  d,
		 w,  h,  d,
	};
	int tris[12*3] = {
		0, 4, 6,
		0, 6, 2,
	 	1, 3, 7,
		1, 7, 5,
		0, 1, 5,
		0, 5, 4,
		2, 6, 7,
		2, 7, 3,
		0, 2, 3,
		0, 3, 1,	
		4, 5, 7,
		4, 7, 6,
	};
	srBegin(GL_TRIANGLES);
	int tri;
	for(tri = 0; tri < 12; ++tri) {
		float* v0 = verts + tris[tri*3+0] * 3;
		float* v1 = verts + tris[tri*3+1] * 3;
		float* v2 = verts + tris[tri*3+2] * 3;
		float n0[3];
		geom_calc_normal(v0, v1, v2, n0);
		srNormal3rv(n0);
		srVertex3rv(v0);
		srVertex3rv(v1);
		srVertex3rv(v2);
	}
	srEnd();
}

void srDrawCylinder(float baseRadius, float topRadius, float height, int slices, int stacks) {
	int i, j;
	float angle;
	float zLow, zHigh;
  float deltaRadius;
	float xyNormalRatio;
	float zNormal;
  float radiusLow, radiusHigh;
	float length;

	srComment("begin cylinder");

	// clamp slices
  if (slices >= CACHE_SIZE) 
		slices = CACHE_SIZE-1;

	// check params
  if (slices < 2 || stacks < 1 || baseRadius < 0.0 || topRadius < 0.0 || height < 0.0) {
		srError(SR_INVALID_VALUE);
		return;
  }

  /* Compute lengths (needed for normal calculations) */
  deltaRadius = baseRadius - topRadius;
  length = sqrt(deltaRadius*deltaRadius + height*height);
  if (length == 0.0) {
		srError(SR_INVALID_VALUE);
		return;
  }
  xyNormalRatio = height / length;

	// create ring vert geometry
  float sinCache[CACHE_SIZE];
  float cosCache[CACHE_SIZE];
  for (i = 0; i < slices; i++) {
		angle = 2 * M_PI * i / slices;
		sinCache[i] = sin(angle);
		cosCache[i] = cos(angle);
  }

	// create normal geometry
	float sinCache3[CACHE_SIZE];
	float cosCache3[CACHE_SIZE];
	for (i = 0; i < slices; i++) {
	  angle = 2 * M_PI * (i-0.5) / slices;
		sinCache3[i] = xyNormalRatio * sin(angle);
		cosCache3[i] = xyNormalRatio * cos(angle);
	}

	// generate triangles
	for (j = 0; j < stacks; j++) {
		srComment("begin stack %d", j);
		zLow = j * height / stacks;
		zHigh = (j + 1) * height / stacks;
		radiusLow = baseRadius - deltaRadius * ((float) j / stacks);
		radiusHigh = baseRadius - deltaRadius * ((float) (j + 1) / stacks);
    zNormal = deltaRadius / length;
		srBegin(GL_TRIANGLES);
		for (i = 0; i < slices; i++) {
			srComment("begin slice %d", i);
			int k = i==slices-1 ? 0 : i+1;

			srNormal3r(sinCache3[i], cosCache3[i], zNormal);
			srVertex3r(radiusLow * sinCache[i], radiusLow * cosCache[i], zLow);
			srVertex3r(radiusHigh * sinCache[i], radiusHigh * cosCache[i], zHigh);
			srVertex3r(radiusLow * sinCache[k], radiusLow * cosCache[k], zLow);

			srVertex3r(radiusLow * sinCache[k], radiusLow * cosCache[k], zLow);
			srVertex3r(radiusHigh * sinCache[i], radiusHigh * cosCache[i], zHigh);
			srVertex3r(radiusHigh * sinCache[k], radiusHigh * cosCache[k], zHigh);
			srComment("end slice %d", i);
		}
		glEnd();
		srComment("end stack %d", j);
	}
	srComment("end cylinder");
}
