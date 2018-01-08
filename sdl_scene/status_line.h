#ifndef _status_line_h_
#define _status_line_h_

struct status_line {
	char *name;
	char *label;
	char *val;
	int row;
	int col;
	float color[3];
};

struct status_line *status_line_construct(
	const char *name, 
	const char *label, 
	const char *val, 
	int row);
void status_line_destroy(struct status_line *self);
void status_line_set(struct status_line *self, const char* val);
void status_line_draw(struct status_line *self);

#endif // _status_line_h_
