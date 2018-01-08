#include <math.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

#include "geom.h"

#define MIN(a, b) ((a)<(b) ? (a) : (b))
#define MAX(a, b) ((a)>(b) ? (a) : (b))

static float epsilon = 1e-7;
//float epsilon = 0.0000001;

float geom_deg2rad(float angle_deg) {
	return M_PI * angle_deg / 180;
}

float geom_rad2deg(float angle_rad) {
	return 180 * angle_rad / M_PI;
}

void geom_frame_tostring(const Frame* frm, char* s_ret) {
	sprintf(s_ret, 
	"trans: (% f, % f, % f)\n  rot: (% f, % f, % f\n        % f, % f, % f\n        % f, % f, % f)", 
		frm->origin[0], frm->origin[1], frm->origin[2], 
		frm->basis[0], frm->basis[1], frm->basis[2], 
		frm->basis[3], frm->basis[4], frm->basis[5], 
		frm->basis[6], frm->basis[7], frm->basis[8]);
}

void geom_bbox_tostring(const Bbox* bbox, char* s_ret) {
	sprintf(s_ret, 
	"min: (% f, % f, % f)\n  max: (% f, % f, % f\n)", 
		bbox->min[0], bbox->min[1], bbox->min[2], 
		bbox->max[0], bbox->max[1], bbox->max[2]);
}

void geom_calc_normal(const float* v0, const float* v1, const float* v2, float* vnret) {
	float v01[3];
	geom_vector3_sub(v1, v0, v01);
	float v12[3];
	geom_vector3_sub(v2, v1, v12);
	geom_vector3_cross(v01, v12, vnret);
	geom_vector3_normalize(vnret);
}

void geom_midpoint(const float* v0, const float* v1, float* vret) {
	geom_vector3_add(v0, v1, vret);
	geom_vector3_scalar_mul(vret, 0.5, vret);
}

/* Returns [a, b, c, d] where:
			ax + by + cz + d = 0 

	plane normal = [a, b, c]
	d = -n dot P, where P is a point in the plane

*/
int geom_get_tri_plane(const float* tri, float* plane_ret) {
	// calc normal
	float vn[3];
	geom_calc_normal(tri+0*3, tri+1*3, tri+2*3, vn);
	float d = -geom_vector3_dot(vn, tri+0*3);
	geom_vector3_copy(vn, plane_ret);
	plane_ret[3] = d;
	return GEOM_NO_ERROR; 
}

/* 
	from gtcg, page 483:

	t = -(n dot P0 + d) / (n dot v) where

		P0 = first point in line segment
		P1 = second point in line segment
		v = P1 - P0
		n = plane normal
		d = d in eqn of plane 'ax + by + cy + d'

	Q = P0 + tv

	Returns:
		1 if line segment intersects plane, else 0


*/
int geom_line_triplane_intersect(const float* line, const float* tri, float* intersect_pt_ret) {
	float plane[4];
	geom_get_tri_plane(tri, plane);

	const float* P0 = line+0*3;
	const float* P1 = line+1*3;
	float v[3];
	geom_vector3_sub(P1, P0, v);
	float n[3];
	geom_vector3_copy(plane, n);
	float d = plane[3];
	float denom = geom_vector3_dot(n, v);
	if(denom == 0)
		return 0;

	float t = - (geom_vector3_dot(n, P0) + d) / denom;
	if(t < 0 || t > 1)
		return 0;
	float tv[3];			
	geom_vector3_scalar_mul(v, t, tv);
	float Q[3];
	geom_vector3_add(P0, tv, Q);
	
	geom_vector3_copy(Q, intersect_pt_ret);
	return 1;
}

/* from ctcg page 486:

| t |                   1                 | ((P - V0) cross (V1 - V0)) dot (V2 - V0)
| u | = _________________________________ | (d cross (V2 - V0)) dot (P - V0)
| v |   (d cross (V2 - V0)) dot (V1 - V0) | ((P - V0) cross (V1 - V0)) dot d

from rtr, page 749:

| t |                   1                 | r dot e2 |
| u | = _________________________________ | q dot s  |
| v | =              q dot e1             | r dot d  |

where
	e1 = T1 - T0
	e2 = T2 - T0
	d = P1 - P0
	s = P0 - T0
  q = d cross e2
	r = s cross e1

Returns:
	if line segment intersects tri: 1
	else: 0
*/
int geom_line_tri_intersect(const float* line, const float* tri, float* intersect_pt_ret) {
int report_no_intersect = 0;
	float P0[3];
	geom_vector3_copy(line + 3*0, P0);
	float P1[3];
	geom_vector3_copy(line + 3*1, P1);
	float d[3];
	geom_vector3_sub(P1, P0, d);

	float T0[3];
	geom_vector3_copy(tri + 3*0, T0);
	float T1[3];
	geom_vector3_copy(tri + 3*1, T1);
	float T2[3];
	geom_vector3_copy(tri + 3*2, T2);

	float e1[3];
	geom_vector3_sub(T1, T0, e1);
	float e2[3];
	geom_vector3_sub(T2, T0, e2);

	float q[3];
	geom_vector3_cross(d, e2, q);

	float s[3];
	geom_vector3_sub(P0, T0, s);

	float a = geom_vector3_dot(q, e1);
	if(fabs(a) < epsilon) {
		if(report_no_intersect) 
			printf("  geom_line_tri_intersect(): no intersect, a < epsilon: %f", a);
		return 0;
	}
	a = 1.0 / a;

	float r[3];
	geom_vector3_cross(s, e1, r);

	float t = a * geom_vector3_dot(r, e2);
	if(t < 0 || t > 1) {
		if(report_no_intersect) 
			printf("  geom_line_tri_intersect(): no intersect, t = %f\n", t);
		return 0;
	}

	float u = a * geom_vector3_dot(q, s);
	if(u < 0) {
		if(report_no_intersect) 
			printf("  geom_line_tri_intersect(): no intersect, u = %f\n", u);
		return 0;
	}

	float v = a * geom_vector3_dot(r, d);
	if(v < 0 || (u + v) > 1) {
		if(report_no_intersect) 
			printf("  geom_line_tri_intersect(): no intersect, v = %f\n", v);
		return 0;
	}

	float td[3];
	geom_vector3_scalar_mul(d, t, td);
	float Q[3];
	geom_vector3_add(P0, td, Q);
	geom_vector3_copy(Q, intersect_pt_ret);
//printf("  intersect! t: %f, u: %f, v: %f, ipt: (%f, %f, %f)\n", t, u, v, Q[0], Q[1], Q[2]);
	return 1;
}

// --- vector3
float geom_vector3_magnitude(const float* v) {
	return sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
}

int geom_vector3_set(float* vret, float x, float y, float z) {
	vret[0] = x;
	vret[1] = y;
	vret[2] = z;
	return GEOM_NO_ERROR; 
}

int	geom_vector3_scalar_mul(const float* v, float s, float* vret) {
	vret[0] = v[0] * s; 
	vret[1] = v[1] * s; 
	vret[2] = v[2] * s;
	return GEOM_NO_ERROR;
}

int geom_vector3_copy(const float* v, float* vret) {
	return geom_vector3_set(vret, v[0], v[1], v[2]);
}

int geom_vector3_zero(float* vret) {
	return geom_vector3_set(vret, 0, 0, 0);
}

int geom_vector3_add(const float* v0, const float* v1, float* vret) {
	int i;
	for(i = 0; i < 3; ++i)
		vret[i] = v0[i] + v1[i];
	return GEOM_NO_ERROR; 
}

int geom_vector3_sub(const float* v0, const float* v1, float* vret) {
	int i;
	for(i = 0; i < 3; ++i)
		vret[i] = v0[i] - v1[i];
	return GEOM_NO_ERROR; 
}

float geom_vector3_distance(const float* v0, const float* v1) {
	float v[3];
	geom_vector3_sub(v0, v1, v);
	return geom_vector3_magnitude(v);
}

float geom_vector3_dot(const float* v0, const float* v1) {
	return v0[0]*v1[0] + v0[1]*v1[1] + v0[2]*v1[2];
}

int geom_vector3_cross(const float* v0, const float* v1, float* vret) {
	float vr[3];
	vr[0] = v0[1]*v1[2] - v0[2]*v1[1];
	vr[1] = v0[2]*v1[0] - v0[0]*v1[2];
	vr[2] = v0[0]*v1[1] - v0[1]*v1[0];
	geom_vector3_copy(vr, vret);
	return GEOM_NO_ERROR; 
}

int geom_vector3_normalize(float* vret) {
	float m = geom_vector3_magnitude(vret);
	if(m == 0)
		geom_vector3_zero(vret);
	else {
		float vn[3];
		int i;
		for(i = 0; i < 3; ++i)
			vn[i] = vret[i] / m;
		geom_vector3_copy(vn, vret);
	}
	return GEOM_NO_ERROR; 
}
void geom_vector3_tostring(const float* v, char* s_ret) {
	sprintf(s_ret, 
	"%f %f %f", 
	v[0], v[1], v[2]);
}

int geom_vector3_equal(const float* v0, const float* v1) {
	return v0[0]==v1[0] && v0[1]==v1[1] && v0[2]==v1[2];
}

int geom_vector3_min(const float* v0, const float* v1, float* vmin_ret) {
	int i;
	for(i = 0; i < 3; ++i)
		vmin_ret[i] = MIN(v0[i], v1[i]);
	return GEOM_NO_ERROR; 
}

int geom_vector3_max(const float* v0, const float* v1, float* vmax_ret) {
	int i;
	for(i = 0; i < 3; ++i)
		vmax_ret[i] = MAX(v0[i], v1[i]);
	return GEOM_NO_ERROR; 
}

int geom_vector3_midpoint(const float* v0, const float* v1, float* vmid_ret) {
	int i;
	for(i = 0; i < 3; ++i)
		vmid_ret[i] = (v0[i] + v1[i]) / 2;
	return GEOM_NO_ERROR; 
}

// --- vector4
int geom_vector4_set(float* vret, float x, float y, float z, float w) {
	vret[0] = x;
	vret[1] = y;
	vret[2] = z;
	vret[3] = w;
	return GEOM_NO_ERROR; 
}
int geom_vector4_copy(const float* v, float* vret) {
	return geom_vector4_set(vret, v[0], v[1], v[2], v[3]);
}

// --- matrix3
int geom_matrix3_set(float* m, 
	float m0, float m1, float m2,
	float m3, float m4, float m5,
	float m6, float m7, float m8
	) {
	m[0] = m0;
	m[1] = m1;
	m[2] = m2;
	m[3] = m3;
	m[4] = m4;
	m[5] = m5;
	m[6] = m6;
	m[7] = m7;
	m[8] = m8;
	return GEOM_NO_ERROR;
}

int geom_matrix3_copy(const float* m, float* mret) {
	int i;
	for(i = 0; i < 9; ++i)
		mret[i] = m[i];
	return GEOM_NO_ERROR; 
}

int geom_matrix3_transposed(const float* m, float* mret) {
	float mr[9];
	mr[ 0] = m[ 0]; mr[ 1] = m[ 3]; mr[ 2] = m[ 6];
	mr[ 3] = m[ 1]; mr[ 4] = m[ 4]; mr[ 5] = m[ 7];
	mr[ 6] = m[ 2]; mr[ 7] = m[ 5]; mr[ 8] = m[ 8];
	geom_matrix3_copy(mr, mret);
	return GEOM_NO_ERROR;
}

int	geom_matrix3_mul(const float* m0, const float* m1, float* mat_ret) {
	float mr[9];
	mr[ 0] = m0[ 0]*m1[ 0] + m0[ 1]*m1[ 3] + m0[ 2]*m1[ 6];
	mr[ 1] = m0[ 0]*m1[ 1] + m0[ 1]*m1[ 4] + m0[ 2]*m1[ 7];
	mr[ 2] = m0[ 0]*m1[ 2] + m0[ 1]*m1[ 5] + m0[ 2]*m1[ 8];

	mr[ 3] = m0[ 3]*m1[ 0] + m0[ 4]*m1[ 3] + m0[ 5]*m1[ 6];
	mr[ 4] = m0[ 3]*m1[ 1] + m0[ 4]*m1[ 4] + m0[ 5]*m1[ 7];
	mr[ 5] = m0[ 3]*m1[ 2] + m0[ 4]*m1[ 5] + m0[ 5]*m1[ 8];

	mr[ 6] = m0[ 6]*m1[ 0] + m0[ 7]*m1[ 3] + m0[ 8]*m1[ 6];
	mr[ 7] = m0[ 6]*m1[ 1] + m0[ 7]*m1[ 4] + m0[ 8]*m1[ 7];
	mr[ 8] = m0[ 6]*m1[ 2] + m0[ 7]*m1[ 5] + m0[ 8]*m1[ 8];

	geom_matrix3_copy(mr, mat_ret);
	return GEOM_NO_ERROR;
}

int	geom_vector3_matrix3_mul(const float* v, const float* m, float* vret) {
	float vr[3];
	vr[0] = v[0]*m[ 0] + v[1]*m[ 3] + v[2]*m[ 6];
	vr[1] = v[0]*m[ 1] + v[1]*m[ 4] + v[2]*m[ 7];
	vr[2] = v[0]*m[ 2] + v[1]*m[ 5] + v[2]*m[ 8];
	geom_vector3_copy(vr, vret);
	return GEOM_NO_ERROR;
}

int	geom_matrix3_vector3_mul(const float* m, const float* v, float* vret) {
	float vr[3];
	vr[0] = m[0]*v[ 0] + m[1]*v[ 1] + m[2]*v[ 2];
	vr[1] = m[3]*v[ 0] + m[4]*v[ 1] + m[5]*v[ 2];
	vr[2] = m[6]*v[ 0] + m[7]*v[ 1] + m[8]*v[ 2];
	geom_vector3_copy(vr, vret);
	return GEOM_NO_ERROR;
}

// Returns an identity matrix
int geom_matrix3_new_ident(float* mat_ret) {
	mat_ret[ 0] = 1; mat_ret[ 1] = 0; mat_ret[ 2] = 0;
	mat_ret[ 3] = 0; mat_ret[ 4] = 1; mat_ret[ 5] = 0;
	mat_ret[ 6] = 0; mat_ret[ 7] = 0; mat_ret[ 8] = 1;
	return GEOM_NO_ERROR;
}

// Returns a matrix with rotation about x axis by param angle angle_deg
int geom_matrix3_new_rotx(float angle_deg, float* mat_ret) {
	float theta = geom_deg2rad(angle_deg);
	mat_ret[ 0] = 1; mat_ret[ 1] = 0; mat_ret[ 2] = 0;
	mat_ret[ 3] = 0; mat_ret[ 4] = cos(theta); mat_ret[ 5] = sin(theta);
	mat_ret[ 6] = 0; mat_ret[ 7] = -sin(theta); mat_ret[ 8] = cos(theta);
	return GEOM_NO_ERROR;
}

// Returns a matrix with rotation about y axis by param angle angle_deg
int geom_matrix3_new_roty(float angle_deg, float* mat_ret) {
	float theta = geom_deg2rad(angle_deg);
	mat_ret[ 0] = cos(theta); mat_ret[ 1] = 0; mat_ret[ 2] = -sin(theta);
	mat_ret[ 3] = 0; mat_ret[ 4] = 1; mat_ret[ 5] = 0;
	mat_ret[ 6] = sin(theta); mat_ret[ 7] = 0; mat_ret[ 8] = cos(theta);
	return GEOM_NO_ERROR;
}

// Returns a matrix with rotation about z axis by param angle angle_deg
int geom_matrix3_new_rotz(float angle_deg, float* mat_ret) {
	float theta = geom_deg2rad(angle_deg);
	mat_ret[ 0] = cos(theta); mat_ret[ 1] = sin(theta); mat_ret[ 2] = 0;
	mat_ret[ 3] = -sin(theta); mat_ret[ 4] = cos(theta); mat_ret[ 5] = 0;
	mat_ret[ 6] = 0; mat_ret[ 7] = 0; mat_ret[ 8] = 1;
	return GEOM_NO_ERROR;
}

// Returns a matrix with rotation about param vector vrot by param angle angle_deg
int geom_matrix3_new_rot(const float* vrot, float angle_deg, float* mat_rot_ret) {
	/* implementation:
	 create Mt * Rx * M where: 
			M is identity matrix transformed to (vrot, vrot_perp, vrot x vrot_perp)
			Rx is rotation matrix about cartesian x axis
	*/
	if( geom_vector3_magnitude(vrot) < epsilon )
		return GEOM_ERR_ZERO_VECTOR;
	float vr[3];
	geom_vector3_copy(vrot, vr);	
	geom_vector3_normalize(vr);
	float vc[3] = {1, 0, 0};
	float vperp[3];
	geom_vector3_cross(vr, vc, vperp);
	if( geom_vector3_magnitude(vperp) < epsilon ) {
		geom_vector3_set(vc, 0, 1, 0);
		geom_vector3_cross(vr, vc, vperp);
	}
	geom_vector3_normalize(vperp);
	float vperp2[3];
	geom_vector3_cross(vr, vperp, vperp2);
	geom_vector3_normalize(vperp2);
	float M[9] = {
		vr[0], vr[1], vr[2],
		vperp[0], vperp[1], vperp[2],
		vperp2[0], vperp2[1], vperp2[2],
	};
	float Rx[9];
	geom_matrix3_new_rotx(angle_deg, Rx);
	float Mi[9];
	geom_matrix3_mul(Rx, M, Mi);
	float Mt[9];
	geom_matrix3_transposed(M, Mt);
	geom_matrix3_mul(Mt, Mi, mat_rot_ret);
	return GEOM_NO_ERROR;
}

void geom_matrix3_tostring(const float* m, char* s_ret) {
	int i;
	s_ret[0] = '\0';
	for(i = 0; i < 9; ++i) {
		char num[128];
		if(i && i % 3 == 0)
			strcat(s_ret, "\n");
		sprintf(num, "%f ", m[i]);
		strcat(s_ret, num);
	}
};

// ===== quaternion
int geom_quaternion_set(float* q, float x, float y, float z, float w) {
	q[0] = x;
	q[1] = y;
	q[2] = z;
	q[3] = w;
	return GEOM_NO_ERROR;
}

int geom_quaternion_copy(const float* q0, float* q1) {
	q1[0] = q0[0];
	q1[1] = q0[1];
	q1[2] = q0[2];
	q1[3] = q0[3];
	return GEOM_NO_ERROR;
}

double geom_quaternion_length_squared(const float* q) {
	return q[0]*q[0] + q[1]*q[1] + q[2]*q[2] + q[3]*q[3];
}

int geom_quaternion_to_matrix3(const float* q, float* mat_ret)
{
  float d = geom_quaternion_length_squared(q);
  assert(d != 0.0);
  float s = 2.0 / d;
  float xs = q[0] * s,   ys = q[1] * s,   zs = q[2] * s;
  float wx = q[3] * xs,  wy = q[3] * ys,  wz = q[3] * zs;
  float xx = q[0] * xs,  xy = q[0] * ys,  xz = q[0] * zs;
  float yy = q[1] * ys,  yz = q[1] * zs,  zz = q[2] * zs;
  geom_matrix3_set(mat_ret,
    1.0 - (yy + zz), xy + wz, xz - wy,
		xy - wz, 1.0 - (xx + zz), yz + wx,
		xz + wy, yz - wx, 1.0 - (xx + yy));
	return GEOM_NO_ERROR;
}

int geom_quaternion_from_matrix3(const float* m, float* q_ret)
{
  float trace = m[0] + m[4] + m[8];
  float quat[4];
  
  if(trace > 0.0) 
  {
    float s = sqrt(trace + 1.0);
    quat[3] = (s * 0.5);
    s = 0.5 / s;
		quat[0] = (m[7] - m[5]) * s;
		quat[1] = (m[6] - m[2]) * s;
		quat[2] = (m[3] - m[1]) * s;
  } 
  else 
  {
		int i = m[0] < m[4] ?
			(m[4] < m[8] ? 2 : 1) :
			(m[0] < m[8] ? 2 : 0);

    int j = (i + 1) % 3;  
    int k = (i + 2) % 3;
    
		float s = sqrt(m[i*3 + i] - m[j*3 + j] - m[k*3 + k] + 1.0);

    quat[i] = s * 0.5;
    s = 0.5 / s;
    
		quat[3] = (m[k*3 + j] - m[j*3 + k]) * s;
		quat[j] = (m[j*3 + i] + m[i*3 + j]) * s;
		quat[k] = (m[k*3 + i] + m[i*3 + k]) * s;
  }
  geom_quaternion_set(q_ret, quat[0], quat[1], quat[2], quat[3]);
	return GEOM_NO_ERROR;
}

int geom_quaternion_slerp(const float* q0, const float* q1, float t, float* qr) {
	float dp = q0[0]*q1[0] + q0[1]*q1[1] + q0[2]*q1[2] + q0[3]*q1[3];
	float theta = acos(dp);
	if(theta < epsilon) {
		return GEOM_ERR_ZERO_VECTOR;
	}
	float a = sin(theta*(1-t)) / sin(theta);
	float b = sin(theta*t) / sin(theta);
	int i;
	for(i = 0; i < 4; ++i) {
		qr[i] = a*q0[i] + b*q1[i];
	}
	return GEOM_NO_ERROR;
}

// ===== matrix4
int geom_matrix4_new_ident(float* m) {
	m[ 0] = 1; m[ 1] = 0; m[ 2] = 0; m[ 3] = 0;	
	m[ 4] = 0; m[ 5] = 1; m[ 6] = 0; m[ 7] = 0;	
	m[ 8] = 0; m[ 9] = 0; m[10] = 1; m[11] = 0;	
	m[12] = 0; m[13] = 0; m[14] = 0; m[15] = 1;	
	return GEOM_NO_ERROR;
}

int geom_matrix4_transposed(const float* m, float* mret) {
	mret[ 0] = m[ 0]; mret[ 1] = m[ 4]; mret[ 2] = m[ 8]; mret[ 3] = m[12];	
	mret[ 4] = m[ 1]; mret[ 5] = m[ 5]; mret[ 6] = m[ 9]; mret[ 7] = m[13];	
	mret[ 8] = m[ 2]; mret[ 9] = m[ 6]; mret[10] = m[10]; mret[11] = m[14];	
	mret[12] = m[ 3]; mret[13] = m[ 7]; mret[14] = m[11]; mret[15] = m[15];	
	return GEOM_NO_ERROR;
}

int geom_matrix3_matrix4_copy(const float* m3, float* m) {
	m[ 0] = m3[ 0]; m[ 1] = m3[ 1]; m[ 2] = m3[ 2];
	m[ 4] = m3[ 3]; m[ 5] = m3[ 4]; m[ 6] = m3[ 5];
	m[ 8] = m3[ 6]; m[ 9] = m3[ 7]; m[10] = m3[ 8];
	return GEOM_NO_ERROR;
}

int geom_matrix4_matrix3_copy(const float* m4, float* m) {
	m[ 0] = m4[ 0]; m[ 1] = m4[ 1]; m[ 2] = m4[ 2];
	m[ 3] = m4[ 4]; m[ 4] = m4[ 5]; m[ 5] = m4[ 6];
	m[ 6] = m4[ 8]; m[ 7] = m4[ 9]; m[ 8] = m4[10];
	return GEOM_NO_ERROR;
}

int geom_vector3_matrix4_copy(const float* v3, float* m) {
	m[12] = v3[0]; m[13] = v3[1]; m[14] = v3[2];
	return GEOM_NO_ERROR;
}

int geom_matrix4_copy(const float* m4, float* m) {
	memcpy(m, m4, sizeof(float) * 16);
	return GEOM_NO_ERROR;
}

int geom_matrix4_vector4_mul(const float* m, const float* v, float* vret) {

	vret[0] = m[ 0]*v[0] + m[ 1]*v[1] + m[ 2]*v[2] + m[ 3]*v[3];
	vret[1] = m[ 4]*v[0] + m[ 5]*v[1] + m[ 6]*v[2] + m[ 7]*v[3];
	vret[2] = m[ 8]*v[0] + m[ 9]*v[1] + m[10]*v[2] + m[11]*v[3];
	vret[3] = m[12]*v[0] + m[13]*v[1] + m[14]*v[2] + m[15]*v[3];
	return GEOM_NO_ERROR;
}

void geom_matrix4_tostring(const float* m, char* s_ret) {
	int i;
	s_ret[0] = '\0';
	for(i = 0; i < 16; ++i) {
		char num[128];
		if(i && i % 4 == 0)
			strcat(s_ret, "\n");
		sprintf(num, "%f ", m[i]);
		strcat(s_ret, num);
	}
};

int geom_vector4_matrix4_mul(const float* v, const float* m, float* vec4_ret) {
	vec4_ret[0] = v[0]*m[ 0] + v[1]*m[ 4] + v[2]*m[ 8] + v[3]*m[ 12];
	vec4_ret[1] = v[0]*m[ 1] + v[1]*m[ 5] + v[2]*m[ 9] + v[3]*m[ 13];
	vec4_ret[2] = v[0]*m[ 2] + v[1]*m[ 6] + v[2]*m[10] + v[3]*m[ 14];
	vec4_ret[3] = v[0]*m[ 3] + v[1]*m[ 7] + v[2]*m[11] + v[3]*m[ 15];
	return GEOM_NO_ERROR;
}

float geom_distance_point_line(const float* point, const float* line_beg, const float* line_end) {
	float line_mag = geom_vector3_distance(line_beg, line_end);
	float u = 
		((point[0] - line_beg[0]) * (line_end[0] - line_beg[0]) +
		(point[1] - line_beg[1]) * (line_end[1] - line_beg[1]) +
		(point[2] - line_beg[2]) * (line_end[2] - line_beg[2])) / (line_mag * line_mag);

	if(u < 0.0f || u > 1.0f)
			return -1;   // closest point does not fall within the line segment
 
	float intersect[3];
	intersect[0] = line_beg[0] + u * (line_end[0] - line_beg[0]);
	intersect[1] = line_beg[1] + u * (line_end[1] - line_beg[1]);
	intersect[2] = line_beg[2] + u * (line_end[2] - line_beg[2]);

	float d = geom_vector3_distance(point, intersect);
	return d;
}


