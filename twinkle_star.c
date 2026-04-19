#include <ncurses.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>

#define STAR_COUNT 100

typedef struct {
    int y, x;
    char symbol;
} Star;

// Global flag for the signal handler
volatile sig_atomic_t keep_running = 1;

void handle_sigint(int sig) {
    keep_running = 0;
}

int main() {
    // Handle Ctrl-C gracefully
    signal(SIGINT, handle_sigint);

    // Initialize ncurses
    initscr();
    noecho();
    curs_set(0);      // Hide the cursor
    timeout(100);    // Non-blocking getch (refresh every 100ms)
    srand(time(NULL));

    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);

    Star stars[STAR_COUNT];

    // Initialize stars with random positions
    for (int i = 0; i < STAR_COUNT; i++) {
        stars[i].y = rand() % max_y;
        stars[i].x = rand() % max_x;
        stars[i].symbol = '.';
    }

    time_t last_twinkle = time(NULL);

    while (keep_running) {
        // Handle terminal resizing
        int cur_y, cur_x;
        getmaxyx(stdscr, cur_y, cur_x);
        if (cur_y != max_y || cur_x != max_x) {
            max_y = cur_y;
            max_x = cur_x;
            clear(); // Clear if screen resized
        }

        // Every 2 seconds, toggle a random star
        if (difftime(time(NULL), last_twinkle) >= 2.0) {
            int i = rand() % STAR_COUNT;
            stars[i].symbol = (stars[i].symbol == '.') ? '+' : '.';
            last_twinkle = time(NULL);
        }

        // Draw the stars
        erase(); // Clear buffer for redrawing
        for (int i = 0; i < STAR_COUNT; i++) {
            // Ensure stars stay within bounds if window shrunk
            if (stars[i].y < max_y && stars[i].x < max_x) {
                mvaddch(stars[i].y, stars[i].x, stars[i].symbol);
            }
        }
        refresh();

        // Check for 'q' to quit manually, or wait for timeout
        if (getch() == 'q') break;
    }

    // Clean up
    endwin();
    printf("Clear skies! Exiting...\n");
    return 0;
}
