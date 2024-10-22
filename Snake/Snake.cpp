#include <iostream>
#include <ncurses.h> // For getch(), and keyboard handling
#include <unistd.h>  // For usleep()
#include <cstdlib>   // For rand() and srand()


using namespace std;




bool gameOver;
const int width = 20;
const int height = 17;
int x, y, fruitX, fruitY, score;
int tailX[100], tailY[100];
int nTail;
int gameSpeed = 100000; // Default game speed (lower is faster)
enum eDirection { STOP = 0, LEFT, RIGHT, UP, DOWN };
eDirection dir;

void Setup() {
    gameOver = false;
    dir = STOP;
    x = width / 2;
    y = height / 2;
    fruitX = rand() % width;
    fruitY = rand() % height;
    score = 0;
    nTail = 0;
}

void Draw() {
    clear();

    // Draw border with double lines
    attron(COLOR_PAIR(4));
    for (int i = 0; i < width + 2; i++) {
        mvprintw(0, i, "=");
        mvprintw(height + 1, i, "=");
    }
    for (int i = 0; i < height + 2; i++) {
        mvprintw(i, 0, "=");
        mvprintw(i, width + 1, "=");
    }
    attroff(COLOR_PAIR(4));

    // Draw snake and fruit with colors
    attron(COLOR_PAIR(1));
    mvprintw(y + 1, x + 1, "O"); // Snake head

    attron(COLOR_PAIR(2));
    mvprintw(fruitY + 1, fruitX + 1, "F"); // Fruit

    attron(COLOR_PAIR(3));
    for (int i = 0; i < nTail; i++) {
        mvprintw(tailY[i] + 1, tailX[i] + 1, "o"); // Snake tail
    }
    attroff(COLOR_PAIR(3));

    // Draw score
    mvprintw(height + 3, 0, "Score: %d", score);
    refresh();
}

void Input() {
    timeout(0);
    int ch = getch();
    switch (ch) {
    case 'a':
        if (dir != RIGHT) dir = LEFT;
        break;
    case 'd':
        if (dir != LEFT) dir = RIGHT;
        break;
    case 'w':
        if (dir != DOWN) dir = UP;
        break;
    case 's':
        if (dir != UP) dir = DOWN;
        break;
    case 'x':
        gameOver = true;
        break;
    }
}

void Logic() {
    int prevX = tailX[0];
    int prevY = tailY[0];
    int prev2X, prev2Y;
    tailX[0] = x;
    tailY[0] = y;
    for (int i = 1; i < nTail; i++) {
        prev2X = tailX[i];
        prev2Y = tailY[i];
        tailX[i] = prevX;
        tailY[i] = prevY;
        prevX = prev2X;
        prevY = prev2Y;
    }
    switch (dir) {
    case LEFT:
        x--;
        break;
    case RIGHT:
        x++;
        break;
    case UP:
        y--;
        break;
    case DOWN:
        y++;
        break;
    default:
        break;
    }

    // Check for boundary collisions
    if (x >= width) x = 0; else if (x < 0) x = width - 1;
    if (y >= height) y = 0; else if (y < 0) y = height - 1;

    // Check for collision with itself
    for (int i = 0; i < nTail; i++) {
        if (tailX[i] == x && tailY[i] == y)
            gameOver = true;
    }

    // Check if fruit is eaten
    if (x == fruitX && y == fruitY) {
        score += 10;
        fruitX = rand() % width;
        fruitY = rand() % height;
        nTail++;
    }
}

void ShowGameOver() {
    clear();
    attron(COLOR_PAIR(5));
    mvprintw(height / 2, (width / 2) - 5, "GAME OVER");
    mvprintw(height / 2 + 1, (width / 2) - 8, "Final Score: %d", score);
    mvprintw(height / 2 + 3, (width / 2) - 10, "Press 'q' to Exit...");
    refresh();
    attroff(COLOR_PAIR(5));

    // Wait for the user to press 'q' to quit
    while (getch() != 'q');
}

void ShowInstructions() {
    clear();
    attron(COLOR_PAIR(5));
    mvprintw(height / 2 - 2, (width / 2) - 10, "Welcome to Snake Game!");
    mvprintw(height / 2, (width / 2) - 12, "Use 'W', 'A', 'S', 'D' to move.");
    mvprintw(height / 2 + 1, (width / 2) - 12, "Press 'x' to quit the game.");
    mvprintw(height / 2 + 3, (width / 2) - 14, "Press any key to start...");
    refresh();
    attroff(COLOR_PAIR(5));

    getch(); // Wait for key press to start the game
}

int main() {
    // Initialize ncurses
    initscr();
    clear();
    noecho();
    cbreak();
    curs_set(0);

    // Initialize color pairs
    if (has_colors() == FALSE) {
        endwin();
        cout << "Your terminal does not support color" << endl;
        return 1;
    }
    start_color();
    init_pair(1, COLOR_GREEN, COLOR_BLACK); // Snake head color
    init_pair(2, COLOR_RED, COLOR_BLACK);   // Fruit color
    init_pair(3, COLOR_YELLOW, COLOR_BLACK);// Snake tail color
    init_pair(4, COLOR_CYAN, COLOR_BLACK);  // Border color
    init_pair(5, COLOR_MAGENTA, COLOR_BLACK); // Instructions/Game Over text

    // Seed random number generator
    srand(time(0));

    ShowInstructions();
    Setup();
    while (!gameOver) {
        Draw();
        Input();
        Logic();
        usleep(gameSpeed); // Control the game speed
    }

    ShowGameOver();

    // End ncurses mode
    endwin();
    return 0;
}
