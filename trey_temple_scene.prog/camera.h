#ifndef _camera_h_
#define _camera_h_

#ifdef __cplusplus
extern "C" {
#endif

// sets camera parameters
void camera_set(const float* eye, const float* cen, const float* up);

// returns camera parameters
void camera_get(float* eye_ret, float* cen_ret, float* up_ret);

// call from glut display func, does gluLookAt() for camera pos
void camera_lookat();

// call from glut keyboard func, returns true if camera key used
int camera_keyboard(unsigned char key, int x, int y);

// call from glut passive motion func, returns true if mouse input used
int camera_passive_motion(int x, int y);

// util function, sets perspective projection (optional, not needed for camera functionality)
void camera_set_projection();

#ifdef __cplusplus
}
#endif

#endif
