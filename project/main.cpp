#include <GL/glut.h>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <utility>
#include <iostream> // For debugging
#include <string>

// Define constants
const int GRID_WIDTH = 20;
const int GRID_HEIGHT = 20;
const int CELL_SIZE = 25;

// Directions
enum Direction { UP, DOWN, LEFT, RIGHT };

// Game states
enum GameState { START_SCREEN, RUNNING, GAME_OVER };
GameState gameState = START_SCREEN;

// Snake and food position
std::vector<std::pair<int, int>> snake = { {10, 10} };
Direction snakeDirection = RIGHT;
std::pair<int, int> food = {15, 15};
bool gameOver = false;
int score = 0;
int highScore = 0;

// Initialize OpenGL
void initOpenGL() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, GRID_WIDTH * CELL_SIZE, 0, GRID_HEIGHT * CELL_SIZE);
}

// Draw a single cell
void drawCell(int x, int y, float r, float g, float b) {
    glColor3f(r, g, b);
    glBegin(GL_QUADS);
    glVertex2i(x * CELL_SIZE, y * CELL_SIZE);
    glVertex2i((x + 1) * CELL_SIZE, y * CELL_SIZE);
    glVertex2i((x + 1) * CELL_SIZE, (y + 1) * CELL_SIZE);
    glVertex2i(x * CELL_SIZE, (y + 1) * CELL_SIZE);
    glEnd();
}

// Generate random food position
void generateFood() {
    food.first = rand() % GRID_WIDTH;
    food.second = rand() % GRID_HEIGHT;
}

// Display text on the screen
void displayText(float x, float y, const std::string& text, void* font = GLUT_BITMAP_HELVETICA_18) {
    glRasterPos2f(x, y);
    for (char c : text) {
        glutBitmapCharacter(font, c);
    }
}

// Display the start screen
void displayStartScreen() {
    glClear(GL_COLOR_BUFFER_BIT);

    displayText(GRID_WIDTH * CELL_SIZE / 4, GRID_HEIGHT * CELL_SIZE / 2, "Developed By\nPress Enter to Start Game");

    glutSwapBuffers();
}

// Display the game over screen
void displayGameOverScreen() {
    glClear(GL_COLOR_BUFFER_BIT);

    displayText(GRID_WIDTH * CELL_SIZE / 4, GRID_HEIGHT * CELL_SIZE / 2 + 20, "Game Over!");
    displayText(GRID_WIDTH * CELL_SIZE / 4, GRID_HEIGHT * CELL_SIZE / 2, "Press Enter to Play Again");

    std::string scoreText = "Score: " + std::to_string(score);
    std::string highScoreText = "High Score: " + std::to_string(highScore);
    displayText(GRID_WIDTH * CELL_SIZE / 4, GRID_HEIGHT * CELL_SIZE / 2 - 20, scoreText);
    displayText(GRID_WIDTH * CELL_SIZE / 4, GRID_HEIGHT * CELL_SIZE / 2 - 40, highScoreText);

    glutSwapBuffers();
}

// Display function
void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    if (gameState == START_SCREEN) {
        displayStartScreen();
        return;
    } else if (gameState == GAME_OVER) {
        displayGameOverScreen();
        return;
    }

    // Draw the food
    drawCell(food.first, food.second, 1.0f, 0.0f, 0.0f);

    // Draw the snake
    for (const auto& segment : snake) {
        drawCell(segment.first, segment.second, 0.0f, 1.0f, 0.0f);
    }

    glutSwapBuffers();
}
void update(int value);
// Reset the game state
void resetGame() {
    snake = { {10, 10} };
    snakeDirection = RIGHT;
    generateFood();
    gameOver = false;
    score = 0;
    gameState = RUNNING;
    glutPostRedisplay();
    glutTimerFunc(100, update, 0);
}

// Update the game state
void update(int value) {
    if (gameState == GAME_OVER) {
        glutPostRedisplay();
        return;
    }

    if (gameState != RUNNING || gameOver) {
        gameOver = true;
        gameState = GAME_OVER;
        highScore = std::max(highScore, score);
        glutPostRedisplay();
        return;
    }

    // Determine new head position
    auto head = snake.front();
    switch (snakeDirection) {
        case UP:    head.second++; break;
        case DOWN:  head.second--; break;
        case LEFT:  head.first--; break;
        case RIGHT: head.first++; break;
    }

    // Check for collisions with walls
    if (head.first < 0 || head.first >= GRID_WIDTH || head.second < 0 || head.second >= GRID_HEIGHT) {
        gameOver = true;
        gameState = GAME_OVER;
        highScore = std::max(highScore, score);
        glutPostRedisplay();
        return;
    }

    // Check for collisions with itself
    for (const auto& segment : snake) {
        if (segment == head) {
            gameOver = true;
            gameState = GAME_OVER;
            highScore = std::max(highScore, score);
            glutPostRedisplay();
            return;
        }
    }

    // Add the new head
    snake.insert(snake.begin(), head);

    // Check if food is eaten
    if (head == food) {
        score += 10;
        generateFood();
    } else {
        // Remove the tail
        snake.pop_back();
    }

    // Redraw the screen
    glutPostRedisplay();
    glutTimerFunc(100, update, 0);
}


// Handle keyboard input
void handleKeys(unsigned char key, int x, int y) {
    if (gameState == START_SCREEN && key == 13) { // Enter key
        std::cout << "Enter key pressed. Starting game..." << std::endl;
        resetGame();
        return;
    }

    if (gameState == GAME_OVER && key == 13) { // Enter key
        std::cout << "Enter key pressed. Restarting game..." << std::endl;
        resetGame();
        return;
    }

    if (gameState == RUNNING) {
        switch (key) {
            case 'w': if (snakeDirection != DOWN) snakeDirection = UP; break;
            case 's': if (snakeDirection != UP) snakeDirection = DOWN; break;
            case 'a': if (snakeDirection != RIGHT) snakeDirection = LEFT; break;
            case 'd': if (snakeDirection != LEFT) snakeDirection = RIGHT; break;
        }
    }
}

void handleSpecialKeys(int key, int x, int y) {
    if (gameState == RUNNING) {
        switch (key) {
            case GLUT_KEY_UP:    if (snakeDirection != DOWN) snakeDirection = UP; break;
            case GLUT_KEY_DOWN:  if (snakeDirection != UP) snakeDirection = DOWN; break;
            case GLUT_KEY_LEFT:  if (snakeDirection != RIGHT) snakeDirection = LEFT; break;
            case GLUT_KEY_RIGHT: if (snakeDirection != LEFT) snakeDirection = RIGHT; break;
        }
    }
}

int main(int argc, char** argv) {
    srand(static_cast<unsigned>(time(0)));

    // Initialize GLUT
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(GRID_WIDTH * CELL_SIZE, GRID_HEIGHT * CELL_SIZE);
    glutCreateWindow("Snake Game");

    initOpenGL();

    // Set up callbacks
    glutDisplayFunc(display);
    glutKeyboardFunc(handleKeys);
    glutSpecialFunc(handleSpecialKeys);

    // Start the main loop
    glutMainLoop();

    return 0;
}