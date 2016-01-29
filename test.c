// Standard Includes
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

// Non Standard Includes
#include <time.h>

// 3rd Party Libraries
#include <ncurses.h>

#define RATE_LIMIT 16.6

double timespec_diff(struct timespec *a, struct timespec *b);

void start_screen(void);
void stop_screen(void);
void refresh_screen(void);

int main(int argc, char *argv[])
{
	int running = 1;

	struct timespec prev, curr;
	double mills_left = 0, mills_till_quit = 5000;

	start_screen();

	clock_gettime(CLOCK_MONOTONIC, &curr);
	prev.tv_sec = curr.tv_sec;
	prev.tv_nsec = curr.tv_nsec;

	printf("Starting the loop\n");

	// The Loop
	while (running) {
		clock_gettime(CLOCK_MONOTONIC, &curr);
		mills_left += timespec_diff(&curr, &prev);

		for ( ; mills_left > RATE_LIMIT && running == 1; mills_left -= RATE_LIMIT) {
			// Something to just quit the program loop after some time
			mills_till_quit -= RATE_LIMIT;
			if (mills_till_quit <= 0) {
				running = 0;
				break;
			}
		}

		refresh_screen();

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

void refresh_screen(void)
{
	refresh();
}
