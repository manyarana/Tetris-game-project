#include <iostream>
#include <vector>
#include <conio.h>
#include <windows.h>
#include <ctime>
#include <string>

using namespace std;
// Constants for game
const int GRID_WIDTH = 10;
const int GRID_HEIGHT = 20;
const int SCREEN_WIDTH = GRID_WIDTH * 2 + 20; // Extra space for score display
const int SCREEN_HEIGHT = GRID_HEIGHT + 4;    // +4 for borders and instructions

// Tetromino shapes and their rotations
const int SHAPES[7][4][4][4] = {
    // I-Shape (Cyan)
    {
        {{0,0,0,0}, {1,1,1,1}, {0,0,0,0}, {0,0,0,0}},
        {{0,0,1,0}, {0,0,1,0}, {0,0,1,0}, {0,0,1,0}},
        {{0,0,0,0}, {0,0,0,0}, {1,1,1,1}, {0,0,0,0}},
        {{0,1,0,0}, {0,1,0,0}, {0,1,0,0}, {0,1,0,0}}
    },
    // O-Shape (Yellow)
    {
        {{0,0,0,0}, {0,1,1,0}, {0,1,1,0}, {0,0,0,0}},
        {{0,0,0,0}, {0,1,1,0}, {0,1,1,0}, {0,0,0,0}},
        {{0,0,0,0}, {0,1,1,0}, {0,1,1,0}, {0,0,0,0}},
        {{0,0,0,0}, {0,1,1,0}, {0,1,1,0}, {0,0,0,0}}
    },
    // T-Shape (Purple)
    {
        {{0,0,0,0}, {0,1,0,0}, {1,1,1,0}, {0,0,0,0}},
        {{0,0,0,0}, {0,1,0,0}, {0,1,1,0}, {0,1,0,0}},
        {{0,0,0,0}, {0,0,0,0}, {1,1,1,0}, {0,1,0,0}},
        {{0,0,0,0}, {0,1,0,0}, {1,1,0,0}, {0,1,0,0}}
    },
    // S-Shape (Green)
    {
        {{0,0,0,0}, {0,1,1,0}, {1,1,0,0}, {0,0,0,0}},
        {{0,0,0,0}, {0,1,0,0}, {0,1,1,0}, {0,0,1,0}},
        {{0,0,0,0}, {0,0,0,0}, {0,1,1,0}, {1,1,0,0}},
        {{0,0,0,0}, {1,0,0,0}, {1,1,0,0}, {0,1,0,0}}
    },
    // Z-Shape (Red)
    {
        {{0,0,0,0}, {1,1,0,0}, {0,1,1,0}, {0,0,0,0}},
        {{0,0,0,0}, {0,0,1,0}, {0,1,1,0}, {0,1,0,0}},
        {{0,0,0,0}, {0,0,0,0}, {1,1,0,0}, {0,1,1,0}},
        {{0,0,0,0}, {0,1,0,0}, {1,1,0,0}, {1,0,0,0}}
    },
    // J-Shape (Blue)
    {
        {{0,0,0,0}, {1,0,0,0}, {1,1,1,0}, {0,0,0,0}},
        {{0,0,0,0}, {0,1,1,0}, {0,1,0,0}, {0,1,0,0}},
        {{0,0,0,0}, {0,0,0,0}, {1,1,1,0}, {0,0,1,0}},
        {{0,0,0,0}, {0,1,0,0}, {0,1,0,0}, {1,1,0,0}}
    },
    // L-Shape (Orange)
    {
        {{0,0,0,0}, {0,0,1,0}, {1,1,1,0}, {0,0,0,0}},
        {{0,0,0,0}, {0,1,0,0}, {0,1,0,0}, {0,1,1,0}},
        {{0,0,0,0}, {0,0,0,0}, {1,1,1,0}, {1,0,0,0}},
        {{0,0,0,0}, {1,1,0,0}, {0,1,0,0}, {0,1,0,0}}
    }
};

// Block characters for drawing the game (fallback if colors don't work)
const char BLOCK_CHARS[2] = {' ', '#'};

// Class for handling console display
class ConsoleHandler {
private:
    HANDLE hConsole;
    COORD cursorPosition;
    bool colorSupported;

public:
    ConsoleHandler() {
        hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        hideCursor();
        enableVirtualTerminalProcessing();
        colorSupported = true;  // Assume color is supported initially
    }

    void hideCursor() {
        CONSOLE_CURSOR_INFO cursorInfo;
        cursorInfo.dwSize = 100;
        cursorInfo.bVisible = FALSE;
        SetConsoleCursorInfo(hConsole, &cursorInfo);
    }

    void enableVirtualTerminalProcessing() {
        // Define ENABLE_VIRTUAL_TERMINAL_PROCESSING if not already defined
        #ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
        #define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
        #endif
        
        // Enable ANSI escape sequences on Windows 10+
        DWORD mode = 0;
        GetConsoleMode(hConsole, &mode);
        SetConsoleMode(hConsole, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    }

    void setCursorPosition(int x, int y) {
        cursorPosition.X = static_cast<SHORT>(x);
        cursorPosition.Y = static_cast<SHORT>(y);
        SetConsoleCursorPosition(hConsole, cursorPosition);
    }

    void clearScreen() {
        system("cls");
    }

    // Draw a colored block at current position
    void drawColorBlock(int colorIndex) {
        if (colorSupported) {
            // Set appropriate color based on tetromino type
            switch (colorIndex) {
                case 0: cout << "  "; break;                    // Empty
                case 1: cout << "\033[46m  \033[0m"; break;     // Cyan (I)
                case 2: cout << "\033[43m  \033[0m"; break;     // Yellow (O)
                case 3: cout << "\033[45m  \033[0m"; break;     // Purple (T)
                case 4: cout << "\033[42m  \033[0m"; break;     // Green (S)
                case 5: cout << "\033[41m  \033[0m"; break;     // Red (Z)
                case 6: cout << "\033[44m  \033[0m"; break;     // Blue (J)
                case 7: cout << "\033[43;1m  \033[0m"; break;   // Orange (L)
                default: cout << "  "; break;
            }
        } else {
            // Fallback to character-based rendering if colors not supported
            cout << BLOCK_CHARS[colorIndex == 0 ? 0 : 1] << BLOCK_CHARS[colorIndex == 0 ? 0 : 1];
        }
    }
};

// Class representing a Tetromino piece
class Tetromino {
private:
    int type;       // Shape type (0-6)
    int rotation;   // Current rotation (0-3)
    int x, y;       // Position on grid

public:
    Tetromino(int t) : type(t), rotation(0), x(GRID_WIDTH / 2 - 2), y(0) {}
    
    // Copy constructor
    Tetromino(const Tetromino& other) : type(other.type), rotation(other.rotation), x(other.x), y(other.y) {}

    int getType() const { return type; }
    int getRotation() const { return rotation; }
    int getX() const { return x; }
    int getY() const { return y; }

    void rotate() { rotation = (rotation + 1) % 4; }
    void moveLeft() { x--; }
    void moveRight() { x++; }
    void moveDown() { y++; }
    
    // Set position directly
    void setPosition(int newX, int newY) {
        x = newX;
        y = newY;
    }
    
    // Set rotation directly
    void setRotation(int newRotation) {
        rotation = newRotation % 4;
    }

    // Check if a specific cell of the tetromino is filled
    bool isFilled(int cellX, int cellY) const {
        // Convert board coordinates to shape coordinates
        int shapeX = cellX - x;
        int shapeY = cellY - y;

        // Check if the coordinates are within shape bounds
        if (shapeX >= 0 && shapeX < 4 && shapeY >= 0 && shapeY < 4) {
            return SHAPES[type][rotation][shapeY][shapeX] == 1;
        }
        return false;
    }

    // Method for the ghost piece preview
    int findGhostDropY(const vector<vector<int>>& grid) const {
        int ghostY = y;
        while (true){
            bool canDrop = true;
            // Check if the piece can drop to the next row
            for (int i=0; i<4; i++){
                for (int j=0; j<4; j++){
                    if (SHAPES[type][rotation][i][j]){
                        int gridX = x + j;
                        int gridY = ghostY + i + 1;
                        // check grid boundaries and collisions
                        if (gridY >= GRID_HEIGHT ||(gridX >= 0 && gridX < GRID_WIDTH && gridY >= 0 && grid[gridY][gridX] != 0)){
                            canDrop = false;
                            break;
                        } 
                    }
                }
                if (!canDrop) break;
            }
            if (!canDrop) break;
            ghostY++;
        }
        return ghostY-1; //Return the last valid Y position
    }
};

// The main Tetris game class
class TetrisGame {
private:
    ConsoleHandler console;
    vector<vector<int>> grid;   // Game grid (0=empty, 1-7=filled with tetromino type)
    Tetromino* currentPiece;
    Tetromino* nextPiece;
    Tetromino* heldPiece; // for hold piece functionality
    bool gameOver;
    int score;
    int level;
    int linesCleared;
    int speed;  // Frames per gravity drop
    int lastSpeedUpdateLines;
    bool canHold; // prevent multiple holds per piece
    
    // Initialize a new random tetromino
    Tetromino* getRandomTetromino() {
        int randomType = rand() % 7;
        return new Tetromino(randomType);
    }

    // Check if the current piece can move to a new position
    bool canMove(int newX, int newY, int newRotation) {
        // Create a temporary tetromino to test the move
        Tetromino temp(*currentPiece);
        temp.setPosition(newX, newY);
        temp.setRotation(newRotation);
        
        // Check if the new position is valid
        for (int y = 0; y < 4; y++) {
            for (int x = 0; x < 4; x++) {
                if (SHAPES[temp.getType()][temp.getRotation()][y][x]) {
                    int gridX = temp.getX() + x;
                    int gridY = temp.getY() + y;

                    // Out of bounds check
                    if (gridX < 0 || gridX >= GRID_WIDTH || gridY >= GRID_HEIGHT) {
                        return false;
                    }

                    // Collision with placed pieces (but only check cells that are within the grid)
                    if (gridY >= 0 && grid[gridY][gridX] != 0) {
                        return false;
                    }
                }
            }
        }
        return true;
    }

    // Lock the current piece in place
    void lockPiece() {
        for (int y = 0; y < 4; y++) {
            for (int x = 0; x < 4; x++) {
                if (SHAPES[currentPiece->getType()][currentPiece->getRotation()][y][x]) {
                    int gridX = currentPiece->getX() + x;
                    int gridY = currentPiece->getY() + y;
                    
                    if (gridY >= 0 && gridY < GRID_HEIGHT && gridX >= 0 && gridX < GRID_WIDTH) {
                        grid[gridY][gridX] = currentPiece->getType() + 1; // +1 because 0 is empty
                    }
                }
            }
        }
    }

    // Check for and clear full lines
    void clearLines() {
        int lines = 0;
        
        for (int y = 0; y < GRID_HEIGHT; y++) {
            bool lineFull = true;
            
            // Check if the line is full
            for (int x = 0; x < GRID_WIDTH; x++) {
                if (grid[y][x] == 0) {
                    lineFull = false;
                    break;
                }
            }
            
            // If the line is full, mark for clearing
            if (lineFull) {
                lines++;
                
                // Move all lines above down
                for (int moveY = y; moveY > 0; moveY--) {
                    for (int x = 0; x < GRID_WIDTH; x++) {
                        grid[moveY][x] = grid[moveY - 1][x];
                    }
                }
                
                // Clear the top line
                for (int x = 0; x < GRID_WIDTH; x++) {
                    grid[0][x] = 0;
                }
                
                // Stay on the same line as it now contains what was previously above
                y--;
            }
        }
        
        // Update score and level based on lines cleared
        if (lines > 0) {
            // Classic Tetris scoring
            switch (lines) {
                case 1: score += 40 * (level + 1); break;
                case 2: score += 100 * (level + 1); break;
                case 3: score += 300 * (level + 1); break;
                case 4: score += 1200 * (level + 1); break;
            }
            
            linesCleared += lines;
            
            // Update level (every 10 lines)
            int newLevel = linesCleared / 10;
            if (newLevel > level){
                level = newLevel;
                updateSpeed();
            }
        }
    }

    // New method for hold piece functionality
    void holdPiece(){
        if (!canHold) return;
        if (heldPiece == nullptr){
            // first time holding. just swap with next piece
            heldPiece = new Tetromino(*currentPiece);
            spawnNewPiece();
        } else {
            // swap current piece with held piece
            Tetromino* temp = currentPiece;
            currentPiece = new Tetromino(*heldPiece);
            delete heldPiece;
            heldPiece = temp;

            // Reset piece position
            currentPiece->setPosition(GRID_WIDTH / 2 - 2, 0);
        }
        canHold = false;
    }    

    // Update speed based on level and lines cleared
    void updateSpeed(){
        // base speed formula: starts at 30 frames per drop at level 0
        // and gradually decreases to a minimun of 5 frames per drop
        speed = 30 - (level * 1.5);
        if (speed < 5) speed = 5;
        if (linesCleared > 50) speed -= 1;
        if (linesCleared > 100) speed -= 1;
        if (speed<3) speed = 3; //ensures we don't go below absolute minimum
    }

    // Check for game over condition
    void checkGameOver() {
        // Game is over if any placed blocks are in the top row
        for (int x = 0; x < GRID_WIDTH; x++) {
            if (grid[0][x] != 0) {
                gameOver = true;
                return;
            }
        }
        
        // Also check if the new piece can be placed
        if (!canMove(currentPiece->getX(), currentPiece->getY(), currentPiece->getRotation())) {
            gameOver = true;
        }
    }

    // Draw the game board and UI
    void draw() {
        console.setCursorPosition(0,0);
        if (gameOver){
            showGameOverScreen();
            return;
        }

        // Draw top border
        cout << "+";
        for (int x = 0; x < GRID_WIDTH * 2; x++) cout << "-";
        cout << "+";

        // Draw game information
        console.setCursorPosition(GRID_WIDTH * 2 + 3, 0);
        console.setCursorPosition(GRID_WIDTH * 2 + 3, 0);
        cout << "TETRIS";
        console.setCursorPosition(GRID_WIDTH * 2 + 3, 2);
        cout << "Score: " << score;
        console.setCursorPosition(GRID_WIDTH * 2 + 3, 3);
        cout << "Level: " << level;
        console.setCursorPosition(GRID_WIDTH * 2 + 3, 4);
        cout << "Lines cleared: " << linesCleared;

        // Draw next piece preview
        console.setCursorPosition(GRID_WIDTH * 2 + 3, 6);
        cout << "Next Piece:";
        for (int y = 0; y < 4; y++) {
            console.setCursorPosition(GRID_WIDTH * 2 + 3, 7 + y);
            for (int x = 0; x < 4; x++) {
                if (SHAPES[nextPiece->getType()][0][y][x]) {
                    console.drawColorBlock(nextPiece->getType() + 1);
                } else {
                    cout << "  ";
                }
            }
        }

        // Draw controls
        console.setCursorPosition(GRID_WIDTH * 2 + 3, 12);
        cout << "Controls:";
        console.setCursorPosition(GRID_WIDTH * 2 + 3, 13);
        cout << "Left/Right: Move";
        console.setCursorPosition(GRID_WIDTH * 2 + 3, 14);
        cout << "Up: Rotate";
        console.setCursorPosition(GRID_WIDTH * 2 + 3, 15);
        cout << "Down: Soft Drop";
        console.setCursorPosition(GRID_WIDTH * 2 + 3, 16);
        cout << "Space: Hard Drop";
        console.setCursorPosition(GRID_WIDTH * 2 + 3, 17);
        cout << "h: Hold Piece";
        console.setCursorPosition(GRID_WIDTH * 2 + 3, 18);
        cout << "ESC  : Quit";

        // Draw grid
        for (int y = 0; y < GRID_HEIGHT; y++) {
            console.setCursorPosition(0, y + 1);
            cout << "|";
            
            for (int x = 0; x < GRID_WIDTH; x++) {
                if (grid[y][x] != 0) {
                    // Draw locked pieces
                    console.drawColorBlock(grid[y][x]);
                } else if (currentPiece->isFilled(x, y)) {
                    // Draw current piece
                    console.drawColorBlock(currentPiece->getType() + 1);
                } else {
                    // Draw empty space
                    console.drawColorBlock(0);
                }
            }
            
            cout << "|";
        }

        // Draw bottom border
        console.setCursorPosition(0, GRID_HEIGHT + 1);
        cout << "+";
        for (int x = 0; x < GRID_WIDTH * 2; x++) cout << "-";
        cout << "+";

        // Draw ghost piece
        int ghostY = currentPiece->findGhostDropY(grid);
        for (int y=0; y<4; y++){
            for (int x=0; x<4; x++){
                if (SHAPES[currentPiece->getType()][currentPiece->getRotation()][y][x]){
                    int gridX = currentPiece->getX() + x;
                    int gridY = ghostY + y;
                    // Only draw if within grid bounds and not already occupied
                    if (gridY >= 0 && gridY < GRID_HEIGHT && gridX >= 0 && gridX < GRID_WIDTH && grid[gridY][gridX] == 0){
                        console.setCursorPosition(1 + gridX * 2, gridY + 1);
                        cout << "\033[90m--\033[0m"; // simple dashed line for grey ghost piece
                    }
                }
            }
        }
        // Draw held piece preview
        if (heldPiece){
            console.setCursorPosition(GRID_WIDTH * 2 + 3, 20);
            cout << "Held Piece:";
            for (int y=0; y<4; y++){
                console.setCursorPosition(GRID_WIDTH * 2 + 3, 21 + y);
                for (int x=0; x<4; x++){
                    if (SHAPES[heldPiece->getType()][0][y][x]){
                        console.drawColorBlock(heldPiece->getType()+1);
                    } else{
                        cout << " ";
                    }
                }
            }
        }
    }

    // Perform a hard drop
    void hardDrop() {
        while (canMove(currentPiece->getX(), currentPiece->getY() + 1, currentPiece->getRotation())) {
            currentPiece->moveDown();
            score += 2; // Extra points for hard drop
        }
        lockPiece();
        spawnNewPiece();
        clearLines();
        checkGameOver();
    }

    // Spawn a new tetromino piece
    void spawnNewPiece() {
        delete currentPiece;
        currentPiece = nextPiece;
        nextPiece = getRandomTetromino();
        canHold = true; // Allow holding again after spawning new piece
    }

    // Apply gravity to the current piece
    void applyGravity() {
        if (canMove(currentPiece->getX(), currentPiece->getY() + 1, currentPiece->getRotation())) {
            currentPiece->moveDown();
        } else {
            lockPiece();
            spawnNewPiece();
            clearLines();
            checkGameOver();
        }
    }

public:
    TetrisGame() : gameOver(false), score(0), level(0), linesCleared(0), speed(30), lastSpeedUpdateLines(0), heldPiece(nullptr), canHold(true) {
        // Initialize grid
        grid.resize(GRID_HEIGHT, vector<int>(GRID_WIDTH, 0));
        
        // Initialize pieces
        currentPiece = getRandomTetromino();
        nextPiece = getRandomTetromino();
        
        // Seed random generator
        srand(static_cast<unsigned int>(time(nullptr)));
    }

    ~TetrisGame() {
        delete currentPiece;
        delete nextPiece;
        if (heldPiece) delete heldPiece;
    }

    // Reset the game to initial state
    void reset() {
        // Clear grid
        for (int y = 0; y < GRID_HEIGHT; y++) {
            for (int x = 0; x < GRID_WIDTH; x++) {
                grid[y][x] = 0;
            }
        }
        
        // Reset game state
        delete currentPiece;
        delete nextPiece;
        currentPiece = getRandomTetromino();
        nextPiece = getRandomTetromino();
        gameOver = false;
        score = 0;
        level = 0;
        linesCleared = 0;
        speed = 30;
        lastSpeedUpdateLines = 0;

        if (heldPiece){
            delete heldPiece;
            heldPiece = nullptr;
        }
        canHold = true;
    }

    // Show game instructions
    void showIntro() {
        console.clearScreen();
        console.setCursorPosition(5, 5);
        cout << "===== TETRIS =====";
        console.setCursorPosition(5, 7);
        cout << "Controls:";
        console.setCursorPosition(5, 8);
        cout << "Left/Right Arrow: Move tetromino";
        console.setCursorPosition(5, 9);
        cout << "Up Arrow: Rotate tetromino";
        console.setCursorPosition(5, 10);
        cout << "Down Arrow: Soft drop";
        console.setCursorPosition(5, 11);
        cout << "Spacebar: Hard drop";
        console.setCursorPosition(5, 12);
        cout << "h: Hold tetromino";
        console.setCursorPosition(5, 13);
        cout << "ESC: Quit game";
        console.setCursorPosition(5, 15);
        cout << "Press any key to start...";
        
        // Wait for key press
        _getch();
        console.clearScreen();
    }

    void showGameOverScreen() {
        console.clearScreen();
        // Box dimensions
        const int boxWidth = 30;
        const int boxHeight = 8;
        const int boxStartX = (SCREEN_WIDTH - boxWidth) / 2;
        const int boxStartY = (SCREEN_HEIGHT - boxHeight) / 2;
        // Draw top border
        console.setCursorPosition(boxStartX, boxStartY);
        cout << "+";
        for (int x = 0; x < boxWidth - 2; x++) cout << "-";
        cout << "+";
        // Draw sides
        for (int y = 1; y < boxHeight - 1; y++) {
            console.setCursorPosition(boxStartX, boxStartY + y);
            cout << "|";
            console.setCursorPosition(boxStartX + boxWidth - 1, boxStartY + y);
            cout << "|";
        }
        // Draw bottom border
        console.setCursorPosition(boxStartX, boxStartY + boxHeight - 1);
        cout << "+";
        for (int x = 0; x < boxWidth - 2; x++) cout << "-";
        cout << "+";
        // Center text inside box
        const int textOffsetY = 2;
        console.setCursorPosition(boxStartX + (boxWidth - 8)/2, boxStartY + textOffsetY);
        cout << "GAME OVER";
        console.setCursorPosition(boxStartX + (boxWidth - 14)/2, boxStartY + textOffsetY + 1);
        cout << "Final Score: " << score;
        console.setCursorPosition(boxStartX + (boxWidth - 22)/2, boxStartY + textOffsetY + 3);
        cout << "Press any key to restart";
        console.setCursorPosition(boxStartX + (boxWidth - 16)/2, boxStartY + textOffsetY + 4);
        cout << "Press ESC to quit";
    }

    // Main game loop
    void run() {
        int frameCount = 0;
        bool keepRunning = true;

        showIntro();

        while (keepRunning) {
            // Process input
            if (_kbhit()) {
                int key = _getch();
                if (gameOver){
                    // Handle game over input
                    if (key == 27) { // ESC
                        keepRunning = false;
                    } else {
                        reset();
                        console.clearScreen(); // clearing gameover screen after reset
                    }
                    while (_kbhit()) _getch();
                    continue;
                }
                // Handle arrow keys (which return 224 followed by direction code)
                if (key == 224) {
                    key = _getch();
                    switch (key) {
                        case 75: // Left arrow
                            if (canMove(currentPiece->getX() - 1, currentPiece->getY(), currentPiece->getRotation())) {
                                currentPiece->moveLeft();
                            }
                            break;
                        case 77: // Right arrow
                            if (canMove(currentPiece->getX() + 1, currentPiece->getY(), currentPiece->getRotation())) {
                                currentPiece->moveRight();
                            }
                            break;
                        case 72: // Up arrow
                            if (canMove(currentPiece->getX(), currentPiece->getY(), (currentPiece->getRotation() + 1) % 4)) {
                                currentPiece->rotate();
                            }
                            break;
                        case 80: // Down arrow
                            if (canMove(currentPiece->getX(), currentPiece->getY() + 1, currentPiece->getRotation())) {
                                currentPiece->moveDown();
                                score++; // Small bonus for soft drop
                            }
                            break;
                    }
                } else {
                    // Handle regular keys
                    switch (key) {
                        case 27: // ESC
                            if (!gameOver){
                                showGameOverScreen();
                                int key = _getch();
                                if (key == 27){
                                    keepRunning = false;
                                }
                            } else {
                                keepRunning = false;
                            }
                            break;
                        case 32: // Spacebar
                            hardDrop();
                            break;
                        case 'h': // Hold Piece
                        case 'H':
                            holdPiece();
                            break;
                    }
                }
            }

            // Apply gravity (drop every 'speed' frames)
            if (!gameOver && ++frameCount >= speed) {
                frameCount = 0;
                applyGravity();
            }
            if (!gameOver && linesCleared - lastSpeedUpdateLines >= 20){
                lastSpeedUpdateLines = linesCleared - (linesCleared % 20);
                if (speed > 6) speed -= 0.5;
            }
            draw(); // Render the game
            Sleep(16); // Control game speed ~60 FPS
        }
    } 
};

int main() {
    SetConsoleTitleW(L"Tetris Game");
    TetrisGame game;
    game.run();
    return 0;
}
