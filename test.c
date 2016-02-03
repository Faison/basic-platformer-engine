// Standard Includes
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

// Non Standard Includes
#include <time.h>

// 3rd Party Libraries
#include <ncurses.h>

#define RATE_LIMIT 16.6
#define FALL_RATE 4

double timespec_diff(struct timespec *a, struct timespec *b);

void start_screen(void);
void stop_screen(void);
void clear_screen(void);
void refresh_screen(void);
int get_ch(void);

void init_entities(void);
void free_entities(void);

void display_entities(void);
void physics_system(long tick);

int main(int argc, char *argv[])
{
	int running = 1;

	struct timespec prev, curr;
	double mills_left = 0, mills_till_quit = 5000;

	start_screen();
	init_entities();

	clock_gettime(CLOCK_MONOTONIC, &curr);
	prev.tv_sec = curr.tv_sec;
	prev.tv_nsec = curr.tv_nsec;

	printf("Starting the loop\n");

	int entry = 0;
	int render = 1;

	// The Loop
	while (running) {
		clock_gettime(CLOCK_MONOTONIC, &curr);
		mills_left += timespec_diff(&curr, &prev);

		if (mills_left > RATE_LIMIT) {
			render = 1;
		}

		for ( ; mills_left > RATE_LIMIT && running == 1; mills_left -= RATE_LIMIT) {
			// Something to just quit the program loop after some time
			mills_till_quit -= RATE_LIMIT;
			if (mills_till_quit <= 0) {
				running = 0;
				break;
			}

			// Process Input
			while ((entry = get_ch()) != ERR) {
				switch (entry) {
				case KEY_UP:
					// Do Thing
					addch('^');
					break;
				case KEY_DOWN:
					// Do Thing
					addch('V');
					break;
				case KEY_LEFT:
					// Do Thing
					addch('<');
					break;
				case KEY_RIGHT:
					// Do Thing
					addch('>');
					break;
				case KEY_RESIZE:
					// Do Thing
					break;
				default:
					// Do Thing
					break;
				}
			}

			physics_system(RATE_LIMIT);
		}

		if (render) {
			// Uncomment and put before the render code
			clear_screen();

			display_entities();

			refresh_screen();
			render = 0;
		}

		prev.tv_sec = curr.tv_sec;
		prev.tv_nsec = curr.tv_nsec;

		// Sleep for some time so we don't lock up the CPU
		if (mills_left < RATE_LIMIT && running) {
			struct timespec elapsed, tv;
			int was_error = 0;

			elapsed.tv_sec = tv.tv_sec = mills_left / 1000.0;
			elapsed.tv_nsec = tv.tv_nsec = ((int)mills_left % 1000) * 1000000;

			do {
				errno = 0;
				was_error = nanosleep(&tv, &elapsed);
			} while (was_error && (errno == EINTR));
		}
	}

	printf("Loop done\n");

	free_entities();
	stop_screen();

	return 0;
}

double timespec_diff(struct timespec *a, struct timespec *b)
{
	return (((a->tv_sec * 1000000000) + a->tv_nsec) -
		((b->tv_sec * 1000000000) + b->tv_nsec)) / 1000000.0;
}

void start_screen(void)
{
	WINDOW *win = initscr();
	cbreak();
	nodelay(win, TRUE);
	keypad(stdscr, TRUE);
	curs_set(0);
	noecho();
}

void stop_screen(void)
{
	endwin();
}

void clear_screen(void)
{
	clear();
}

void refresh_screen(void)
{
	refresh();
}

int get_ch(void)
{
	return getch();
}

/**
 * Objects
 */
struct entity {
	double pos_x;
	double pos_y;
	char symbol;
	int physics;
	double vel_x;
	double vel_y;
};

struct location_component {
	int entity_id;
	double x;
	double y;
};

struct ascii_render_component {
	int entity_id;
	char symbol;
};

struct physics_component {
	int entity_id;
};

struct velocity_component {
	int entity_id;
	float x_vel;
	float y_Vel;
};

/*
int next_entity_id = 0;
int entities[100];
struct location_component *location_comps = NULL;
struct ascii_render_component *ascii_render_comps = NULL;
struct physics_component *physics_comps = NULL;
struct velocity_component *velocity_comps = NULL;
*/
struct entity **entities = NULL;

void init_entities(void)
{
	int i = 0;
/*
	for (i = 0; i < 100; i++) {
		entities[i] = 0;
	}

	location_comps = calloc(5, sizeof(struct location_component));
	ascii_render_comps = calloc(5, sizeof(struct ascii_render_component));
	physics_comps = calloc(4, sizeof(struct physics_component));
	velocity_comps = calloc(4, sizeof(struct velocity_component));
*/
	entities = calloc(5, sizeof(struct entity *));
	for (i = 0; i < 4; i++) {
		entities[i] = malloc(sizeof(struct entity));
		entities[i]->pos_x = i + 20;
		entities[i]->pos_y = i + 5;
		entities[i]->vel_x = 3 * i + 1;
		entities[i]->vel_y = i - 6;
		entities[i]->physics = 1;
		entities[i]->symbol = '*';
	}
}

void free_entities(void)
{
/*
	if (location_comps != NULL) {
		free(location_comps);
	}

	if (ascii_render_comps != NULL) {
		free(ascii_render_comps);
	}

	if (physics_comps != NULL) {
		free(physics_comps);
	}

	if (velocity_comps != NULL) {
		free(velocity_comps);
	}
*/
	for (int i = 0; entities[i] != NULL; i++) {
		free(entities[i]);
		entities[i] = NULL;
	}

	if (entities != NULL) {
		free(entities);
		entities = NULL;
	}
}

void display_entities(void)
{
	for (int i = 0; entities[i] != NULL; i++) {
		mvaddch((int) entities[i]->pos_y, (int) entities[i]->pos_x, entities[i]->symbol);
	}
}

void physics_system(long tick)
{
	for (int i = 0; entities[i] != NULL; i++) {
		// Fall First
		entities[i]->vel_y += tick / 1000.0 * FALL_RATE;

		// Adjust Pos
		entities[i]->pos_x += tick / 1000.0 * entities[i]->vel_x;
		entities[i]->pos_y += tick / 1000.0 * entities[i]->vel_y;
	}
}
