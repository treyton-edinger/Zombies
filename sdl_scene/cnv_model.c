#include "cnv_model.h"

#include <GL/glut.h>

#include <stdlib.h>
#include <string.h>

struct cnv_model *cnv_model_construct(const char *filename) {
	struct cnv_model *self = malloc(sizeof(struct cnv_model));
	strncpy(self->filename, filename, FILENAME_MAX);
	self->filename[FILENAME_MAX] = '\0';
	self->records = NULL;
	self->num_records = cnv_model_load(self);

	return self;
}

int cnv_model_load(struct cnv_model *self) {
	FILE *fin = fopen(self->filename, "r");
	if (!fin)
		return 0;

	struct cnv_record *prev_record = NULL;
	int num_recs = 0;
	char *line = NULL;
	size_t len;
	int nread;
	while ((nread=getline(&line, &len, fin)) != -1) {
		// skip comments
		if (*line == '#')
			continue;

		char rec_type;
		float x, y, z;
		int nscan = sscanf(line, "%c %f %f %f", &rec_type, &x, &y, &z);
		if (nscan == 4) {
			struct cnv_record *record = malloc(sizeof(struct cnv_record));
			record->record_type = rec_type;
			record->coords[0] = x;
			record->coords[1] = y;
			record->coords[2] = z;
			record->next = NULL;

			if (prev_record == NULL) {
				self->records = record;
				prev_record = record;
			}
			else {
				prev_record->next = record;
				prev_record = record;
			}
			++num_recs;
		}
	}
	free(line);
	fclose(fin);
	return num_recs;
}

void cnv_model_destroy(struct cnv_model *self) {
	struct cnv_record *record = self->records;
	while (record) {
		struct cnv_record *next = record->next;
		free(record);
		record = next;
	}
	free(self);
}

void cnv_model_draw(struct cnv_model *self) {
	glBegin(GL_TRIANGLES);
	struct cnv_record *record = self->records;
	while (record) {
		if (record->record_type == 'c')
			glColor3fv(record->coords);
		else if (record->record_type == 'n')
			glNormal3fv(record->coords);
		else
			glVertex3fv(record->coords);
		record = record->next;
	}
	glEnd();
}

