#ifndef _edit_line_h_
#define _edit_line_h_

struct edit_line {
	char *name;
	char *prompt;
	char *val;
	int row;
	int col;
	int visible;
	const char *(*handler)(const char*);
};

// construct edit line object at param row
struct edit_line *edit_line_construct(const char *name, int row);

// destroy edit line object
void edit_line_destroy(struct edit_line *self);

// activate edit line to handle key input, call handler when input ends with ENTER
void edit_line_prompt(
		struct edit_line *self, 
		const char* prompt, 
		const char *default_val, 
		const char *(*handler)(const char*));

// key input to edit line
int edit_line_keyboard(struct edit_line *self, int key);

// draw edit line
void edit_line_draw(struct edit_line *self);

#endif // _edit_line_h_
