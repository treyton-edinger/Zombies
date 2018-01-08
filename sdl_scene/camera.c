// camera.c	:	camera class

#include "app.h"
#include "camera.h"
#include "geom.h"

//#include <GL/glut.h>
#include <SDL2/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include <stdio.h>
#include <stdlib.h>

#define TRANS_x 'a'
#define TRANS_X 'd'
#define TRANS_y 'f'
#define TRANS_Y 'r'
#define TRANS_z 'w'
#define TRANS_Z 's'

static float eye[] = {0, 5, 10};
static float cen[] = {0, 0, 0};
static float up[] = {0, 1, 0};
static float trans_rate = 1.0;
//static float rot_rate = 5.0;
static float fov = 90;
static float Near = 0.1;
static float Far = 100;
//static float mouse_trans_rate = 0.05;
static float mouse_rot_rate = 0.5;

/*  returns matrix representing param cam rotation where cam up dir is projected onto alignment with world y axis,
    or nil if cam up dir is perpedicular to world y axis
*/
static int get_projected_frame(float* look_dir, float* mat_ret) {
	// convert translation to adjusted camera frame, where adjustment is projection of look_dir onto xz plane,
	// up_dir is aligned with world y axis
	int rc = 0;
	float proj_look_dir[3];
	rc |= geom_vector3_copy(look_dir, proj_look_dir);
	proj_look_dir[1] = 0;
	rc |= geom_vector3_normalize(proj_look_dir);
	if( geom_vector3_magnitude(proj_look_dir) == 0 )
		return GEOM_ERR_ZERO_VECTOR;
	float vy[3] = {0, 1, 0};
	float vx[3];
	rc |= geom_vector3_cross(proj_look_dir, vy, vx);
	float vz[3];
	rc |= geom_vector3_scalar_mul(proj_look_dir, -1, vz);
	float Mr[9] = {vx[0], vx[1], vx[2],
		vy[0], vy[1], vy[2], 
		vz[0], vz[1], vz[2]};
	rc |=geom_matrix3_copy( Mr, mat_ret);
	return rc;
}

static int get_look_dir(float* look_dir_ret) {
	// get look dir
	geom_vector3_sub(cen, eye, look_dir_ret);
	int rc = geom_vector3_normalize(look_dir_ret);
	return rc;
}

static int camera_translate(float* v) {
	// tranform trans vector by current look and up dirs
	int rc;
	float look_dir[3];
	get_look_dir(look_dir);

	float mp[9];
	rc = get_projected_frame(look_dir, mp);
	rc = geom_vector3_matrix3_mul(v, mp, v);
	geom_vector3_add(eye, v, eye);
	geom_vector3_add(cen, v, cen);
	return rc;
}

static void camera_rotate_x(float angle_deg) {

	// get look dir and mag
	float look_dir[3];
	geom_vector3_sub(cen, eye, look_dir);
	float look_mag = geom_vector3_magnitude(look_dir);
	geom_vector3_normalize(look_dir);

	// rotate both look dir and up dir about the x axis
	float vrot[3];
	geom_vector3_cross(look_dir, up, vrot);
	float mrot[9];
	geom_matrix3_new_rot(vrot, angle_deg, mrot);
	geom_vector3_matrix3_mul(look_dir, mrot, look_dir);
	geom_vector3_matrix3_mul(up, mrot, up);

	// project new cen from rotated look_dir
	float new_look[3];
	geom_vector3_set(new_look, look_mag*look_dir[0], look_mag*look_dir[1], look_mag*look_dir[2]);
	geom_vector3_add(eye, new_look, cen);
}

static void camera_rotate_y(float angle_deg) {
	// get look dir and mag
	float look_dir[3];
	geom_vector3_sub(cen, eye, look_dir);
	float look_mag = geom_vector3_magnitude(look_dir);
	geom_vector3_normalize(look_dir);

	// rotate both look dir and up dir about the y axis
	float vy[] = {0, 1, 0};
	float mrot[9];
	geom_matrix3_new_rot(vy, angle_deg, mrot);
	geom_vector3_matrix3_mul(look_dir, mrot, look_dir);
	geom_vector3_matrix3_mul(up, mrot, up);

	// project new cen from rotated look_dir
	float new_look[3];
	geom_vector3_set(new_look, look_mag*look_dir[0], look_mag*look_dir[1], look_mag*look_dir[2]);
	geom_vector3_add(eye, new_look, cen);
}

void camera_set(const float* new_eye, const float* new_cen, const float* new_up) {
	geom_vector3_copy(new_eye, eye);
	geom_vector3_copy(new_cen, cen);
	geom_vector3_copy(new_up, up);
}

void camera_get(float* eye_ret, float* cen_ret, float* up_ret) {
	geom_vector3_copy(eye, eye_ret);
	geom_vector3_copy(cen, cen_ret);
	geom_vector3_copy(up, up_ret);
}

void camera_set_projection() {
//	int win_width = glutGet(GLUT_WINDOW_WIDTH);
//	int win_height = glutGet(GLUT_WINDOW_HEIGHT);
	int win_width = app_win_width();
	int win_height = app_win_height();
	glMatrixMode(GL_PROJECTION);
	float aspect = (float) win_width / (float) win_height;
	gluPerspective(fov, aspect, Near, Far);
	glMatrixMode(GL_MODELVIEW);
}

int camera_keyboard(unsigned char key) {
	switch(key) {
	case TRANS_x: { float v[3] = {-trans_rate, 0, 0}; camera_translate(v); } break;
	case TRANS_X: { float v[3] = {trans_rate, 0, 0}; camera_translate(v); } break;
	case TRANS_y: { float v[3] = {0, -trans_rate, 0}; camera_translate(v); } break;
	case TRANS_Y: { float v[3] = {0, trans_rate, 0}; camera_translate(v); } break;
	case TRANS_z: { float v[3] = {0, 0, -trans_rate}; camera_translate(v); } break;
	case TRANS_Z: { float v[3] = {0, 0, trans_rate}; camera_translate(v); } break;
	default:
		return 0;
	}
	return 1;
}

int camera_passive_motion(int x, int y) {
	// prev motion position should always be at center of window, set with glutWarpPosition
//	int win_w = glutGet(GLUT_WINDOW_WIDTH);
//	int win_h = glutGet(GLUT_WINDOW_HEIGHT);
	int win_w = app_win_width();
	int win_h = app_win_height();
	int prev_x = win_w / 2;
	int prev_y = win_h / 2;

	// calc motion
	int dx = x - prev_x;
	int dy = y - prev_y;

	if (dx == 0 && dy == 0)
		return 1;

//	printf("passive: dx: %d, dy: %d\n", dx, dy);

	camera_rotate_y(-dx * mouse_rot_rate);
	camera_rotate_x(-dy * mouse_rot_rate);

	// get mouse back at center of window
//	glutWarpPointer(prev_x, prev_y);
	app_set_mouse(prev_x, prev_y);

	return 1;
}

void camera_lookat() {
	gluLookAt(
		eye[0], eye[1], eye[2],
		cen[0], cen[1], cen[2],
		up[0], up[1], up[2]);
}

