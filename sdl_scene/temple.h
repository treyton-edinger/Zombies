#ifndef _temple_h_
#define _temple_h_

struct temple {
	float pos[3];
	float size;
};

struct temple *temple_construct(const float *pos, float size);

void temple_draw(struct temple *self);

void temple_destroy(struct temple *self);

#endif
