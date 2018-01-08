#ifndef _geom_h_
#define _geom_h_

#define GEOM_NO_ERROR 0
#define GEOM_ERR_NOT_IMPLEMENTED 1
#define GEOM_ERR_ZERO_VECTOR 2

struct Frame {
	float basis[3*3];
	float origin[3];
};
typedef struct Frame Frame;

struct Bbox {
	float min[3];
	float max[3];
};
typedef struct Bbox Bbox;

// util
float geom_deg2rad(float angle_deg);
float geom_rad2deg(float angle_rad);
void geom_calc_normal(const float* v0, const float* v1, const float* v2, float* vnret);
void geom_midpoint(const float* v0, const float* v1, float* vret);
float geom_distance_point_line(const float* point, const float* line_beg, const float* line_end);
int geom_get_tri_plane(const float* tri, float* plane_ret);

int geom_line_triplane_intersect(const float* line, const float* tri, float* intersect_pt_ret);

/*
	descrip:
		Calculates intersection point of param line and param triangle.

	params:
		line: float[2*3] - array of coords for 2 3D points
		tri:  float[3*3] - array of coords for 3 3D points
		intersect_pt_ret: float[3] - array of coords for 1 3D point

	returns:
		if line segment intersects tri: 1
		else: 0
*/
int geom_line_tri_intersect(const float* line, const float* tri, float* intersect_pt_ret);

// Frame
void geom_frame_tostring(const Frame* frm, char* s_ret);

// Bbox
void geom_bbox_tostring(const Bbox* bbox, char* s_ret);

// vector3
float geom_vector3_magnitude(const float* v);
int geom_vector3_set(float* vret, float x, float y, float z);
int	geom_vector3_scalar_mul(const float* v, float s, float* vret);
int geom_vector3_copy(const float* v, float* vret);
int geom_vector3_zero(float* vret);
int geom_vector3_add(const float* v0, const float* v1, float* vret);
int geom_vector3_sub(const float* v0, const float* v1, float* vret);
float geom_vector3_distance(const float* v0, const float* v1);
float geom_vector3_dot(const float* v0, const float* v1);
int geom_vector3_cross(const float* v0, const float* v1, float* vret);
int geom_vector3_normalize(float* vret);
void geom_vector3_tostring(const float* v, char* s_ret);
int geom_vector3_equal(const float* v0, const float* v1);
int geom_vector3_min(const float* v0, const float* v1, float* vmin_ret);
int geom_vector3_max(const float* v0, const float* v1, float* vmax_ret);

// vector4
int geom_vector4_set(float* vret, float x, float y, float z, float w);
int geom_vector4_copy(const float* v, float* vret);

// matrix3
int geom_matrix3_set(float* m, 
	float m0, float m1, float m2,
	float m3, float m4, float m5,
	float m6, float m7, float m8
	);
int geom_matrix3_transposed(const float* m, float* mret);
int geom_matrix3_copy(const float* m, float* mret);
int	geom_matrix3_mul(const float* m0, const float* m1, float* mr);
int	geom_vector3_matrix3_mul(const float* v, const float* m, float* vret);
int	geom_matrix3_vector3_mul(const float* m, const float* v, float* vret);
int geom_matrix3_new_ident(float* mat_ret);
int geom_matrix3_new_rotx(float angle_deg, float* mat_ret);
int geom_matrix3_new_roty(float angle_deg, float* mat_ret);
int geom_matrix3_new_rotz(float angle_deg, float* mat_ret);
int geom_matrix3_new_rot(const float* vrot, float angle_deg, float* mat_rot_ret);
void geom_matrix3_tostring(const float* m, char* s_ret);

// quaternion
int geom_quaternion_set(float* q, float x, float y, float z, float w);
int geom_quaternion_copy(const float* q0, float* q1);
double geom_quaternion_length_squared(const float* q);
int geom_quaternion_to_matrix3(const float* q, float* mat_ret);
int geom_quaternion_from_matrix3(const float* m, float* q_ret);
int geom_quaternion_slerp(const float* q0, const float* q1, float t, float* qinterp_ret);

// matrix4
int geom_vector3_matrix4_copy(const float* vec3, float* mat4_ret);
int geom_vector4_matrix4_mul(const float* v, const float* m, float* vec4_ret);
int geom_matrix4_new_ident(float* mat_ret);
int geom_matrix3_matrix4_copy(const float* mat3, float* mat4_ret);
int geom_matrix4_matrix3_copy(const float* mat4, float* mat3_ret);
int geom_matrix4_copy(const float* mat4, float* mat4_ret);
int geom_matrix4_vector4_mul(const float* m, const float* v, float* vret);
int geom_matrix4_transposed(const float* m, float* mret);
void geom_matrix4_tostring(const float* m, char* s_ret);

#endif
