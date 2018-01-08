#ifndef _pine_tree_h_
#define _pine_tree_h_

struct pine_tree {
	float pos[3];
	float size;
};

struct pine_tree *pine_tree_construct(const float *pos, float size);

void pine_tree_draw(struct pine_tree *self);

void pine_tree_destroy(struct pine_tree *self);

#endif
