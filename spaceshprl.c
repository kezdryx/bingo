#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h> // For usleep() on Unix/Linux
#include <termios.h> // For non-blocking keyboard input
#include <fcntl.h> // For setting file control options

#define SCREEN_WIDTH 50
#define SCREEN_HEIGHT 10
#define SPACESHIP '>'
#define ALIEN '@'
#define BULLET '^'
#define MAX_ALIENS 5 // Maximum number of aliens on the screen

typedef struct {
    int x;
    int y;
} Alien;

void setTerminalMode(int enable) {
    static struct termios oldt, newt;
    if (enable) {
        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
        fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);
    } else {
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
        fcntl(STDIN_FILENO, F_SETFL, O_RDONLY);
    }
}

void clearScreen() {
    system("clear"); // Clear the console on Unix/Linux systems
}

void drawScreen(char screen[SCREEN_HEIGHT][SCREEN_WIDTH]) {
    for (int i = 0; i < SCREEN_HEIGHT; i++) {
        for (int j = 0; j < SCREEN_WIDTH; j++) {
            putchar(screen[i][j]);
        }
        putchar('\n');
    }
}

int main() {
    char screen[SCREEN_HEIGHT][SCREEN_WIDTH];
    int spaceshipY = SCREEN_HEIGHT / 2;
    int spaceshipX = 5;
    int bulletY = -1, bulletX = -1;
    Alien aliens[MAX_ALIENS];
    int gameOver = 0;

    srand(time(NULL));

    // Initialize aliens
    for (int i = 0; i < MAX_ALIENS; i++) {
        aliens[i].x = SCREEN_WIDTH - 1 - (i * 3); // Stagger aliens
        aliens[i].y = rand() % SCREEN_HEIGHT;
    }

    printf("Use W/S to move, SPACE to shoot, Q to quit.\n");

    setTerminalMode(1); // Enable non-blocking keyboard input

    while (!gameOver) {
        // Clear the screen buffer
        for (int i = 0; i < SCREEN_HEIGHT; i++) {
            for (int j = 0; j < SCREEN_WIDTH; j++) {
                screen[i][j] = ' ';
            }
        }

        // User input controls
        char key = getchar();
        if (key == 'w' || key == 'W') {
            if (spaceshipY > 0) {
                spaceshipY--;
            }
        } else if (key == 's' || key == 'S') {
            if (spaceshipY < SCREEN_HEIGHT - 1) {
                spaceshipY++;
            }
        } else if (key == ' ') {
            if (bulletX == -1) { // Shoot only if no bullet exists
                bulletY = spaceshipY;
                bulletX = spaceshipX + 1;
            }
        } else if (key == 'q' || key == 'Q') {
            break; // Exit game
        }

        // Move bullet
        if (bulletX != -1) {
            bulletX++;
            if (bulletX >= SCREEN_WIDTH) {
                bulletX = -1; // Reset bullet
            }
        }

        // Move aliens
        for (int i = 0; i < MAX_ALIENS; i++) {
            aliens[i].x--;
            if (aliens[i].x < 0) {
                aliens[i].x = SCREEN_WIDTH - 1;
                aliens[i].y = rand() % SCREEN_HEIGHT;
            }
        }

        // Check for collisions
        for (int i = 0; i < MAX_ALIENS; i++) {
            if (aliens[i].x == spaceshipX && aliens[i].y == spaceshipY) {
                gameOver = 1;
            } else if (aliens[i].x == bulletX && aliens[i].y == bulletY) {
                aliens[i].x = SCREEN_WIDTH - 1;
                aliens[i].y = rand() % SCREEN_HEIGHT;
                bulletX = -1; // Destroy bullet
            }
        }

        // Draw elements
        screen[spaceshipY][spaceshipX] = SPACESHIP;
        if (bulletX != -1) {
            screen[bulletY][bulletX] = BULLET;
        }
        for (int i = 0; i < MAX_ALIENS; i++) {
            screen[aliens[i].y][aliens[i].x] = ALIEN;
        }

        // Render the screen
        clearScreen();
        drawScreen(screen);

        // Game over message
        if (gameOver) {
            printf("\nGAME OVER\n");
            break;
        }

        usleep(100000); // Add delay for better gameplay (100ms)
    }

    setTerminalMode(0); // Restore terminal mode
    return 0;
}
