#ifndef _cnv_model_h_
#define _cnv_model_h_

#include <stdio.h>

struct cnv_record {
	char record_type;
	float coords[3];
	struct cnv_record *next;
};

struct cnv_model {
	char filename[FILENAME_MAX + 1];
	struct cnv_record *records;
	int num_records;
};

struct cnv_model *cnv_model_construct(const char *filename);

int cnv_model_load(struct cnv_model *self);

void cnv_model_destroy(struct cnv_model *self);

void cnv_model_draw(struct cnv_model *self);

#endif // _cnv_model_h_
