#include "app.h"
#include "scene.h"
#include "ui.h"
#include "camera.h"
#include "status_line.h"

#include <FTGL/ftgl.h>
#include <SDL2/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <stdio.h>
#include <time.h>
#include <assert.h>
#include <unistd.h>
#include <sys/time.h>

// fwd decls
static void init_ui(void);
static void init_gl(void);
static void init_scene(void);
static void set_projection(void);
static int handle_event(SDL_Event *event);
static void tick(void);
static float calc_frame_rate(void);
static void draw(void);
static void check_gl_errors(void);

// module data

// scene
static struct scene *app_scene = NULL;
//float clear_color[3] = {0.5, 0.66, 0.92};
float clear_color[3] = {0.3, 0.46, 0.72};
// fps
static struct timeval prev_fps_time;
static int prev_frame_count = 0;
static int frame_count = 0;
static float cur_fps = 0;
//static struct timeval prev_tick_time;
//static float target_tick_time = 1.0 / 30.0;
//static int limit_fps = 1;

// ui
static struct ui *app_ui = NULL;

// projection
static float perspective_fovy = 60;
static float perspective_near = 0.1;
static float perspective_far = 5000;

// cam
static float init_eye[] = {11.659676, 8.000000, 39.859119};
static float init_cen[] = {11.096773, 7.480471, 37.758343};
static float init_up[] = {0.198267, 0.642788, 0.739943};

// window
static int window_width = 800;
static int window_height = 600;
static SDL_Window *app_window = NULL;

// module funcs
int main(int argc, char *argv[]) {

	// init sdl
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_EVENTS) != 0) {
		SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
		return 1;
	}

	// Create an application window with the following settings:
	app_window = SDL_CreateWindow(
		"An SDL2 window",                  // window title
		SDL_WINDOWPOS_UNDEFINED,           // initial x position
		SDL_WINDOWPOS_UNDEFINED,           // initial y position
		window_width,                      // width, in pixels
		window_height,                     // height, in pixels
		SDL_WINDOW_OPENGL                  // flags - see below
	);

	// Check that the window was successfully created
	if (app_window == NULL) {
		// In the case that the window could not be made...
		printf("Could not create window: %s\n", SDL_GetError());
		return 1;
	}

	// Create an OpenGL context associated with the window.
	SDL_GLContext glcontext = SDL_GL_CreateContext(app_window);

	init_ui();
	init_gl();
	init_scene();

	// event loop
	int quit = 0;
	while (!quit) {

		// handle events
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			quit = handle_event(&event);
		}

		// update app
		tick();

		// draw
		draw();

	}

       	// Once finished with OpenGL functions, the SDL_GLContext can be deleted.
	SDL_GL_DeleteContext(glcontext);  

	// Close and destroy the window
        SDL_DestroyWindow(app_window);

	SDL_Quit();

	return 0;
}

static void init_ui(void) {
	app_ui = ui_construct();

	// init fps vars
	gettimeofday(&prev_fps_time, NULL);
//	gettimeofday(&prev_tick_time, NULL);

	// put mouse in center of window
	app_set_mouse(window_width/2, window_height/2);
}

static void init_gl(void) {
	// init cam
	camera_set(init_eye, init_cen, init_up);

	// init projection
	set_projection();

	// init color buffer
	glClearColor(clear_color[0], clear_color[1], clear_color[2], 0);

	// lighting
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	float light_dir[4] = {0.3, 1, 0.3, 1};
	glLightfv(GL_LIGHT0, GL_POSITION, light_dir);
	glEnable(GL_NORMALIZE);

	// color
	glEnable(GL_COLOR_MATERIAL);

	// texture
	glEnable(GL_TEXTURE_2D);

	// depth
	glEnable(GL_DEPTH_TEST);
/*
	int depth;
	SDL_GL_GetAttribute(SDL_GL_DEPTH_SIZE, &depth);
	printf("SDL_GL_DEPTH_SIZE: %d\n", depth);
*/
}

static void init_scene(void) {
	app_scene = scene_construct();
}

static void set_projection(void) {
	int win_width = window_width;
	int win_height = window_height;
	glMatrixMode(GL_PROJECTION);
	float fov = perspective_fovy;
	float aspect = (float) win_width / (float) win_height;
	float near_val = perspective_near;
	float far_val = perspective_far;
	gluPerspective(fov, aspect, near_val, far_val);
	glMatrixMode(GL_MODELVIEW);
}

static int handle_event(SDL_Event *event) {
	int quit = 0;

	switch(event->type) {
	case SDL_QUIT:
		quit = 1;
		break;
	case SDL_KEYDOWN: 
		{
			SDL_KeyboardEvent *key_event = (SDL_KeyboardEvent*) event;
			int sym = (int) key_event->keysym.sym;
//			printf("keysym: %d\n", sym);

			if (camera_keyboard(sym))
				break;

			if (sym == SDLK_ESCAPE)
				quit = 1;
		}
		break;
	case SDL_MOUSEMOTION:
		{
			SDL_MouseMotionEvent *mouse_event = (SDL_MouseMotionEvent*) event;
			camera_passive_motion(mouse_event->x, mouse_event->y);
		}
		break;
	default:
		break;
	}
	return quit;
}

static void tick(void) {
	// calc frame rate and update frame rate status
	char txt[256];
	float frame_rate = calc_frame_rate();
	sprintf(txt, "%.1f", frame_rate);
	status_line_set(app_ui->frame_rate_status, txt);
}

float calc_frame_rate() {
	// get current time
	struct timeval now;
	gettimeofday(&now, NULL);

	// get interval from previous calculation time to now
	struct timeval dt;
	timersub(&now, &prev_fps_time, &dt);

	// if interval is at least 1 second, do a new fps calculation
	struct timeval update_interval = {1, 0};
	int update = timercmp(&dt, &update_interval, >);
	if (update) {
		// get interval time and frame count, calculate rate
		float dt_secs = dt.tv_sec + dt.tv_usec * 1e-6;
		float dframes = frame_count - prev_frame_count;
		cur_fps = dframes / dt_secs;
		
		// store current time and frame count as reference, to be used for next fps calculation
		prev_fps_time.tv_sec = now.tv_sec;	
		prev_fps_time.tv_usec = now.tv_usec;	
		prev_frame_count = frame_count;
	}
	return cur_fps;
}

static void draw(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	camera_lookat();

	scene_draw(app_scene);

	ui_draw(app_ui);

	SDL_GL_SwapWindow(app_window);
	check_gl_errors();

	++frame_count;
}

static void check_gl_errors(void) {
	GLenum err;
	while((err = glGetError()) != GL_NO_ERROR) {
		printf("GL Error %d: %s\n", err, gluErrorString(err));
	}
}

// app.h implementation

int app_win_width(void) {
	int w, h;
	SDL_GetWindowSize(app_window, &w, &h);
	return w;
}

int app_win_height(void) {
	int w, h;
	SDL_GetWindowSize(app_window, &w, &h);
	return h;
}

void app_set_mouse(int x, int y) {
	SDL_WarpMouseInWindow(app_window, x, y);
}

