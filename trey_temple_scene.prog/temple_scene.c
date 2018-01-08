// temple_scene.c	:	very simple terrain, sky and temple
#include "ui.h"
#include "camera.h"
#include "temple.h"
#include "pine_tree.h"
#include "fence.h"
#include "guard_tower.h"
#include "road.h"
#include "house.h"
#include "person.h"
#include "zombie.h"

#include "geom.h"

#include <GL/glut.h>

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define FPS 60
#define KEY_ESC 27
#define KEY_SELECT ' '
#define KEY_WIRE 'W'
#define KEY_SHOW_PICKING 'P'

#define NUM_TREES 10
#define ARRAY_SIZE(a) (sizeof(a)/sizeof(a[0]))

#define dbg printf

// module data

// win
static const char* win_title = "temple_scene";
static float cc[4] = {0.0, 0.0, 0.0, 1};	// clear color
static int init_win_width = 800;
static int init_win_height = 600;

// projection
static float perspective_fovy = 60;
static float perspective_near = 0.1;
static float perspective_far = 5000;

// cam
static float init_eye[] = {11.659676, 18.000000, 39.859119};
static float init_cen[] = {11.096773, 17.480471, 37.758343};
static float init_up[] = {0.198267, 0.642788, 0.739943};

// terrain
//static float ground_color[3] = {0.25, 0.566, 0.25};
//static float grid_unit = 10;
//static int num_adj_levels = 2;
static int wireframe = 0;

// sky
static float sky_color[3] = {0.5, 0.66, 0.92};
static float sky_depth_factor = 0.99;

// temple
static float temple_height = 8.84 + 5;

// trees
struct PineTree {
	float pos[3];
	float size;
};
typedef struct PineTree PineTree;
static PineTree trees[NUM_TREES];
static int num_trees = ARRAY_SIZE(trees);

// selection
static int select_mode = 0;
static int show_picking = 0;
static float pick_line[3*2];
static float pick_color[3] = {1, 0, 0};
static float pick_point_radius = 0.05;

// window
static int win_w() {return glutGet(GLUT_WINDOW_WIDTH);}
static int win_h() {return glutGet(GLUT_WINDOW_HEIGHT);}

// ui
static struct ui *app_ui = NULL;

// inline module funcs
static float randf() {
	return (float)rand()/(float)RAND_MAX;
}

// module function declarations

static void init(int argc, char* argv[]);
static void app_destroy(void);
static void create_trees(void);
static void keyboard(unsigned char key, int x, int y);
static void passive_motion(int x, int y);
static void idle(void);
static void draw(void);
static void set_projection(void);
static void check_gl_errors();
static void zombie_draw();

// === init
int main(int argc, char* argv[]) {
	init(argc, argv);

	glutMainLoop();
	return 0;
}

static void init(int argc, char* argv[]) {
	// init glut
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGBA);
	glutInitWindowSize(init_win_width, init_win_height);
	glutCreateWindow(win_title);
	glutKeyboardFunc(keyboard);
	glutPassiveMotionFunc(passive_motion);
	glutIdleFunc(idle);
	glutDisplayFunc(draw);

	// put mouse in center of window
	glutWarpPointer(init_win_width/2, init_win_height/2);

	// init opengl
	glClearColor(cc[0], cc[1], cc[2], cc[3]);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	float light_dir[3] = {1, 1, 1};
	glLightfv(GL_LIGHT0, GL_POSITION, light_dir);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_NORMALIZE);
//	glEnable(GL_CULL_FACE);

	// init cam
	camera_set(init_eye, init_cen, init_up);

	// init projection
	set_projection();

/*
	grid_terrain_set_draw_op(GL_TRIANGLES);
	grid_terrain_set_unit(grid_unit);
	grid_terrain_set_color(round_color);
	grid_terrain_create();
*/
	// init geometry
	create_trees();

	// init ui
	app_ui = ui_construct();

	// init destruction
	atexit(app_destroy);
}

void app_destroy(void) {
	dbg("app_destroy: beg\n");

	ui_destroy(app_ui);

	dbg("app_destroy: end\n");
}

static void create_trees(void) {
	srand(8);
	int i;
	for(i = 0; i < num_trees; ++i) {
		glPushMatrix();
			PineTree* tree = trees + i;
			float minx = -100;
			float minz = -100;
			float b = 200;
			float bx = b;
			float bz = b;
			float x = minx + randf()*bx;
			float y = 0;
			float z = minz + randf()*bz;
			geom_vector3_set(tree->pos, x, y, z);
			float s = 1 + randf() * 3;
			tree->size = s;
		glPopMatrix();
	}
}

// === input
static void keyboard(unsigned char key, int x, int y) {
	if(camera_keyboard(key, x, y)) {
//		grid_terrain_update_cur_sector();
		return;
	}
	switch(key) {
	case KEY_SHOW_PICKING:
		show_picking = !show_picking;
		printf("show_picking is %d\n", show_picking);
		break;
	case KEY_WIRE:
		wireframe = !wireframe;
		printf("wireframe is %d\n", wireframe);
//		grid_terrain_set_draw_op(wireframe?GL_LINE_LOOP:GL_TRIANGLES);
		break;
	case KEY_SELECT:
		select_mode = !select_mode;
		printf("select mode is %d\n", select_mode);
		break;
	case KEY_ESC:
		exit(0);
	default:
		break;
	}
}

static void passive_motion(int x, int y) {
	if(camera_passive_motion(x, y)) {
		return;
	}
}

static void idle(void) {
	glutPostRedisplay();
}

// === drawing
static void draw_sky() {
	float d = perspective_far * sky_depth_factor;
	float w = win_w()/2 * d;
	float h = win_h()/2 * d;
	glDisable(GL_LIGHTING);
	glColor3fv(sky_color);
	glBegin(GL_TRIANGLES);
		glVertex3f(-w, -h, -d);
		glVertex3f( w, -h, -d);
		glVertex3f( w,  h, -d);

		glVertex3f(-w, -h, -d);
		glVertex3f( w,  h, -d);
		glVertex3f(-w,  h, -d);
	glEnd();
	glEnable(GL_LIGHTING);
}

static void draw_trees() {
	int i;
	for(i = 0; i < num_trees; ++i) {
		glPushMatrix();
			PineTree* tree = trees + i;
			glTranslatef(tree->pos[0], tree->pos[1], tree->pos[2]);
//			printf("draw tree at (%f, %f, %f)\n", x, y, z);
			float s = tree->size;
			glScalef(s, s, s);
			pine_tree_draw();
		glPopMatrix();
	}
}

static void draw_pick_line() {
	glColor3fv(pick_color);

	GLUquadric* q = gluNewQuadric();
	float* v = pick_line;
	glPushMatrix();
		glTranslatef(v[0], v[1], v[2]);
		gluSphere(q, pick_point_radius, 16, 16);
	glPopMatrix();
	v += 3;
	glPushMatrix();
		glTranslatef(v[0], v[1], v[2]);
		gluSphere(q, pick_point_radius, 16, 16);
	glPopMatrix();
	gluDeleteQuadric(q);

	glBegin(GL_LINES);
		glVertex3fv(pick_line);
		glVertex3fv(pick_line+3);
	glEnd();
}

static void draw_terrain() {
	float terrain_color[3] = {0.3, 0.2, 0};
	glColor3fv(terrain_color);
	glBegin(GL_QUADS);
		glVertex3f(-100, 0, -100);
		glVertex3f(-100, 0,  100);
		glVertex3f( 100, 0,  100);
		glVertex3f( 100, 0, -100);
	glEnd();
}
float x = 0.0;
float y =0.0;
float dx = 1.1;
float dy = 1.1;
void cir_draw(float cx, float cy, float r, int num_segments) {
	float tri_color[3] = {1, 0, 0};

	float *color = tri_color;
		glColor3fv(color);
	glBegin(GL_POLYGON);

	int i;

        for(i = 0; i < num_segments; i++)
        {
		float theta = 2.0f * 3.1415926f * (float)i / (float)num_segments;//get the current angle

		float x = r * cos(theta);//calculate the x component
		float y = r * sin(theta);//calculate the y component

		glVertex2f(x + cx, y + cy);//output vertex

        }
        glEnd();


}

static void draw_fence(){
	int i=0;
	int rot =90;

	for(i=0;i<4;i++){
		fence_draw();

		glRotatef(rot,0,1,0);
		rot+=90;
	}
}

static void draw_tower(){


glPushMatrix();

		glTranslatef(25,0,25);
		guard_tower_draw();
		glTranslatef(-50,0,0);
		guard_tower_draw();
		glTranslatef(0,0,-50);
		guard_tower_draw();
		glTranslatef(50,0,0);
		guard_tower_draw();
glPopMatrix();

}

static void draw_road() {
	road_draw();
}

static void draw_house(){
	glTranslatef(0,6,0);
	house_draw();
}

static void draw_people(){
float person_color[3]= {0.9,0.9,0.7};
float unlucky_color[3]= {0.255/0.255,0.105/0.255,0.180/0.255};
glPushMatrix();
	glTranslatef(25,30,25);
	draw_person(person_color);
	glTranslatef(-50,0,0);
	draw_person(person_color);
	glTranslatef(0,0,-50);
	draw_person(person_color);
	glTranslatef(50,0,0);
	draw_person(person_color);
	glTranslatef(-14,-30,-6);
	draw_person(person_color);
	glTranslatef(-22,0,0);
	draw_person(person_color);
glPopMatrix();

glPushMatrix();
glTranslatef(-13,2,-95);
glRotatef(90,1,0,0);
draw_person(unlucky_color);
glTranslatef(0,6,1.9);
cir_draw(x, y, 6, 50);

glPopMatrix();

}

static void zombie_draw(){
float zombie_color[3]= {0.2,0.4,0.2};
glPushMatrix();
	glScalef(0.8,0.8,0.8);
	glTranslatef(20,0,-80);
	draw_zomb(zombie_color);
	glTranslatef(-15,0,-30);
	draw_zomb(zombie_color);
	glTranslatef(-20,0, 20);
	draw_zomb(zombie_color);
glPopMatrix();
}

static void draw(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
//	grutil_clear_verts_drawn();

	draw_sky();

	camera_lookat();

	// draw terrain
//	grid_terrain_draw(num_adj_levels);
	draw_terrain();

	// draw trees
	draw_trees();

	//draw fence
	draw_fence();

	//draws guard tower
	draw_tower();

	//draw road
	draw_road();

	//draws people
	draw_people();

	//draws zombie
	zombie_draw();

	// draw house
	glPushMatrix();
		glTranslatef(0, temple_height/2, 0);
		draw_house();
	glPopMatrix();

	// picking
	if(show_picking)
		draw_pick_line();

	// ui
//	ui_draw(app_ui);

	// end draw
	glFlush();
	glutSwapBuffers();

	check_gl_errors();
//	grutil_check_gl_errors();
//	timeutil_inc_frames_drawn();
}

static void check_gl_errors() {
	GLenum err;
	while((err = glGetError()) != GL_NO_ERROR) {
		printf("GL Error %d: %s\n", err, gluErrorString(err));
	}
}

static void set_projection(void) {
	int win_width = glutGet(GLUT_WINDOW_WIDTH);
	int win_height = glutGet(GLUT_WINDOW_HEIGHT);
	glMatrixMode(GL_PROJECTION);
	float fov = perspective_fovy;
	float aspect = (float) win_width / (float) win_height;
	float near_val = perspective_near;
	float far_val = perspective_far;
	gluPerspective(fov, aspect, near_val, far_val);
	glMatrixMode(GL_MODELVIEW);
}
