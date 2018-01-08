#ifndef _image_util_h_
#define _image_util_h_

#define IMAGEUTIL_NOERROR 0
#define IMAGEUTIL_ERROR_ZERO_COMPONENTS 1
#define IMAGEUTIL_ERROR_UNKNOWN_GL_FORMAT 2

struct image {
	int width;
	int height;
	int num_components;
	unsigned char* pixels;
	int gl_format;
};

/* 
	descrip: loads image data from param image file into param image struct
	returns: 0 if success, else IMAGEUTIL_ERROR_XXX
*/
int image_load(const char* fname, struct image* img_ret);

#endif
