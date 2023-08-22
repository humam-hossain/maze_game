#include <iostream>
#include <cstdlib>
#include <ctime>
#include <Windows.h>
#include <chrono>
#include <conio.h>
#include <string>
#include <thread>

#define OFFSET 5
#define ARROW_KEY 224
#define ARROW_UP 72
#define ARROW_RIGHT 77
#define ARROW_DOWN 80
#define ARROW_LEFT 75
#define ENTER_KEY 13
#define BACKSPACE_KEY 8


void clearScreen() {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD coordScreen = { 0, 0 };
    DWORD cCharsWritten;
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    DWORD dwConSize;

    GetConsoleScreenBufferInfo(hConsole, &csbi);
    dwConSize = csbi.dwSize.X * csbi.dwSize.Y;

    FillConsoleOutputCharacter(hConsole, ' ', dwConSize, coordScreen, &cCharsWritten);
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    FillConsoleOutputAttribute(hConsole, csbi.wAttributes, dwConSize, coordScreen, &cCharsWritten);
    SetConsoleCursorPosition(hConsole, coordScreen);
}

class Cell {
public:
    int col;
    int row;
    Cell** parentGrid;
    int parentSize;

    bool visited = false;

    // top, right, bottom, left
    bool walls[4] = { true, true, true, true };

    void removeWalls(Cell current, Cell next) {
        int dirX = this->parentGrid[current.row][current.col].col - parentGrid[next.row][next.col].col;
        int dirY = this->parentGrid[current.row][current.col].row - parentGrid[next.row][next.col].row;

        if (dirX == 1) {
            this->parentGrid[current.row][current.col].walls[3] = false;
            parentGrid[next.row][next.col].walls[1] = false;
        }
        else if (dirX == -1) {
            this->parentGrid[current.row][current.col].walls[1] = false;
            parentGrid[next.row][next.col].walls[3] = false;
        }

        if (dirY == 1) {
            this->parentGrid[current.row][current.col].walls[0] = false;
            parentGrid[next.row][next.col].walls[2] = false;
        }
        else if (dirY == -1) {
            this->parentGrid[current.row][current.col].walls[2] = false;
            parentGrid[next.row][next.col].walls[0] = false;
        }
    }
};

Cell current;

// stack
class Node {
public:
    Cell data;
    Node* next;

    Node(Cell value) {
        this->data = value;
        this->next = nullptr;
    }
};

class Stack {
public:
    Stack() {
        this->top = nullptr;
        this->length = 0;
    }

    void push(Cell value) {
        Node* newNode = new Node(value);
        newNode->next = top;
        top = newNode;
        this->length++;
    }

    Cell pop() {
        Node* temp = this->top;
        top = top->next;
        this->length--;

        return temp->data;
    }

    int getLength() {
        return this->length;
    }

private:
    Node* top;
    int length;
};

class Maze {
public:
    Maze(int size) {
        this->size = size;

        this->grid = (Cell**)malloc(sizeof(Cell*) * size);

        for (int i = 0; i < size; ++i) {
            this->grid[i] = (Cell*)malloc(sizeof(Cell) * size);
        }
    }

    void setup() {
        for (int i = 0; i < this->size; ++i) {
            for (int j = 0; j < this->size; ++j) {
                Cell cell;

                cell.row = i;
                cell.col = j;
                cell.parentGrid = this->grid;
                cell.parentSize = this->size;

                this->grid[i][j] = cell;
            }
        }

        current = this->grid[0][0];
    }

    void setSize(int size) {
        free(this->grid);

        this->size = size;
        this->grid = (Cell**)malloc(sizeof(Cell*) * size);

        for (int i = 0; i < size; ++i) {
            this->grid[i] = (Cell*)malloc(sizeof(Cell) * size);
        }
    }

    Cell checkNeighbours(Cell cell) {
        int row = cell.row;
        int col = cell.col;

        int count = 0;

        Cell* arr = (Cell*)malloc(sizeof(Cell) * 4);

        if (row != 0 && this->grid[row - 1][col].visited == false) {
            arr[count] = this->grid[row - 1][col];
            count++;
        }

        if (col != this->size - 1 && this->grid[row][col + 1].visited == false) {
            arr[count] = this->grid[row][col + 1];
            count++;
        }

        if (row != this->size - 1 && this->grid[row + 1][col].visited == false) {
            arr[count] = this->grid[row + 1][col];
            count++;
        }

        if (col != 0 && this->grid[row][col - 1].visited == false) {
            arr[count] = this->grid[row][col - 1];
            count++;
        }

        if (count != 0) {
            // random neighbour
            int i = rand() % count;
            // std::cout << "(" << arr[i].col << "," << arr[i].row << ")\n";
            return arr[i];
        }

        Cell null;
        null.col = -1;
        null.row = -1;

        return null;
    }

    void generate() {
        this->grid[current.row][current.col].visited = true;

        //this->draw();

        if (checkNeighbours(current).col != -1) {
            Cell next = checkNeighbours(current);
            //std::cout << "(" << next.row << "," << next.col << ")\n";

            next.visited = true;
            this->stack.push(current);

            current.removeWalls(current, next);
            current = next;

            //std::cout << this->stack.getLength() << std::endl;
        }
        else if (this->stack.getLength() > 0) {
            current = this->stack.pop();
        }

        if (this->stack.getLength() == 0) {
            return;
        }

        //clearScreen();

        this->generate();
    }

    void draw(int row, int col, std::string name, int level, std::string time, bool solved=false) {
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), COORD{ (short)OFFSET, (short)OFFSET - 4 });
        std::cout << "Level: " << level;

        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), COORD{ (short)OFFSET, (short)OFFSET - 3 });
        std::cout << "Player: " << name;

        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), COORD{ (short)OFFSET, (short)OFFSET - 2 });
        std::cout << "Duration: " << time;

        if (solved) {
            SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), COORD{ (short)(OFFSET + this->size*2 - 4), (short)OFFSET - 1 });
            std::cout << "Solved!";
        }

        for (int i = 0; i < this->size; ++i) {
            for (int j = 0; j < this->size; ++j) {
                int left = OFFSET + j * 4;
                int top = OFFSET + i * 2;

                SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), COORD{ (short)left, (short)top });
                std::cout << "+   +";
                SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), COORD{ (short)left, (short)(top + 2) });
                std::cout << "+   +";

                // draw top cell wall
                if (this->grid[i][j].walls[0]) {
                    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), COORD{ (short)(left + 1), (short)top });
                    std::cout << "---";
                }

                // draw right cell wall
                if (this->grid[i][j].walls[1]) {
                    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), COORD{ (short)(left + 4), (short)(top + 1) });
                    std::cout << "|";
                }

                // draw bottom cell wall
                if (this->grid[i][j].walls[2]) {
                    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), COORD{ (short)(left + 1), (short)(top + 2) });
                    std::cout << "---";
                }

                // draw bottom cell wall
                if (this->grid[i][j].walls[3]) {
                    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), COORD{ (short)(left), (short)(top + 1) });
                    std::cout << "|";
                }

                // player position
                if (i == row && j == col) {
                    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), COORD{ (short)(left + 2), (short)(top + 1) });
                    std::cout << "*";
                }

                // finish position
                if (i == this->size - 1 && j == this->size - 1) {
                    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), COORD{ (short)(left + 2), (short)(top + 1) });
                    std::cout << "#";
                }

            }
        }

        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), COORD{ (short)(OFFSET), (short)(OFFSET + this->size*2 + 2) });
        std::cout << "<- BackSpace";
    }

    bool isWall(int row, int col, int wallIndex) {
        return this->grid[row][col].walls[wallIndex];
    }

    int getSize() {
        return this->size;
    }

private:
    int size;
    Cell** grid;
    Stack stack;
};

class Player {
public:
    Player(int row, int col, std::string name, int level) {
        this->row = row;
        this->col = col;
        this->name = name;
        this->level = level;
        this->solved = false;
    }

    void menu() {
        clearScreen();

        int pos = 0;
        int key;

        while (true) {
            // header
            SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), COORD{ (short)OFFSET+4, (short)OFFSET });
            std::cout << "MAZE RUNNER";
            SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), COORD{ (short)OFFSET, (short)OFFSET + 1 });
            std::cout << "--------------------";

            // indicator
            SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), COORD{ (short)OFFSET, (short)(OFFSET + 3 + pos) });
            std::cout << "-->";

            // main menu
            SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), COORD{ (short)OFFSET + 4, (short)OFFSET + 3 });
            std::cout << "Play";
            SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), COORD{ (short)OFFSET + 4, (short)OFFSET + 4 });
            std::cout << "Scoreboard";
            SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), COORD{ (short)OFFSET + 4, (short)OFFSET + 5 });
            std::cout << "About";
            SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), COORD{ (short)OFFSET + 4, (short)OFFSET + 6 });
            std::cout << "Exit";

            key = _getch();

            if (key == ARROW_KEY) {
                key = _getch();

                if (key == ARROW_UP && pos != 0) {
                    pos--;
                    clearScreen();
                }
                else if(key == ARROW_DOWN && pos != 3) {
                    pos++;
                    clearScreen();
                }
            }
            else if (key == ENTER_KEY) {
                switch (pos)
                {
                case 0: // play
                    this->play();
                    break;
                case 1: // scoreboard
                    break;
                case 2: // about
                    this->about();
                    break;
                case 3: // exit
                    break;
                }

                SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), COORD{ (short)0, (short)OFFSET + 8 });
                std::cout << "Closing...";
                return;
            }
        }

    }

    void play() {
        std::string time = "00:00:00";
        clearScreen();

        Maze maze(this->level + 5);
        maze.setup();
        maze.generate();
        maze.draw(this->row, this->col, this->name, this->level, time);

        auto startTime = std::chrono::high_resolution_clock::now();

        int key;

        while (true) {
            key = _getch();

            if (key == ARROW_KEY && !this->solved) {
                key = _getch();

                // duration calc
                auto endTime = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime);

                std::string hr = std::to_string(duration.count() / 3600);
                std::string min = std::to_string((duration.count() % 3600) / 60);
                std::string sec = std::to_string(duration.count() % 60);

                if (hr.length() == 1) {
                    hr = "0" + hr;
                }

                if (min.length() == 1) {
                    min = "0" + min;
                }

                if (sec.length() == 1) {
                    sec = "0" + sec;
                }

                time = hr + ":" + min + ":" + sec;

                // player move
                if (key == ARROW_UP && !maze.isWall(this->row, this->col, 0)) {
                    this->row--;
                    clearScreen();
                    maze.draw(this->row, this->col, this->name, this->level, time);
                }
                else if (key == ARROW_RIGHT && !maze.isWall(this->row, this->col, 1)) {
                    this->col++;
                    clearScreen();
                    maze.draw(this->row, this->col, this->name, this->level, time);
                }
                else if (key == ARROW_DOWN && !maze.isWall(this->row, this->col, 2)) {
                    this->row++;
                    clearScreen();
                    maze.draw(this->row, this->col, this->name, this->level, time);
                }
                else if (key == ARROW_LEFT && !maze.isWall(this->row, this->col, 3)) {
                    this->col--;
                    clearScreen();
                    maze.draw(this->row, this->col, this->name, this->level, time);
                }
            }

            // check goal reached
            if (this->row == maze.getSize() - 1 && this->col == maze.getSize() - 1) {
                // show
                this->solved = true;
                clearScreen();
                maze.draw(this->row, this->col, this->name, this->level, time, this->solved);
                std::this_thread::sleep_for(std::chrono::seconds(1));
                
                // increase level
                this->level++;

                // reset for new level
                this->solved = false;
                time = "00:00:00";
                this->row = 0;
                this->col = 0;

                // new level maze
                maze.setSize(this->level + 5);
                maze.setup();
                maze.generate();

                clearScreen();
                maze.draw(this->row, this->col, this->name, this->level, time);
                startTime = std::chrono::high_resolution_clock::now();
            }

            // <- back
            if (key == BACKSPACE_KEY) {
                break;
            }
            
        }

        this->menu();
    }

    void about() {
        clearScreen();

        // header
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), COORD{ (short)OFFSET, (short)OFFSET });
        std::cout << "ABOUT";
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), COORD{ (short)OFFSET, (short)OFFSET + 1 });
        std::cout << "--------------------";

        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), COORD{ (short)OFFSET, (short)OFFSET + 2 });
        std::cout << "Experience a captivating maze game powered by the Depth-First Search (DFS) recursive backtracking algorithm.";
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), COORD{ (short)OFFSET, (short)OFFSET + 3 });
        std::cout << "Traverse through a dynamically generated maze, navigating pathways and walls using arrow keys.This game not";
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), COORD{ (short)OFFSET, (short)OFFSET + 4 });
        std::cout << "only provides entertainment but also offers insights into algorithms, problem - solving, and C++ coding. Embark";
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), COORD{ (short)OFFSET, (short)OFFSET + 5 });
        std::cout << "on a journey of exploration and learn while having fun.";

        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), COORD{ (short)OFFSET, (short)OFFSET + 7 });
        std::cout << "References:";
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), COORD{ (short)OFFSET, (short)OFFSET + 8 });
        std::cout << "Wikipedia: Maze generation algorithm (https://en.wikipedia.org/wiki/Maze_generation_algorithm)";
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), COORD{ (short)OFFSET, (short)OFFSET + 9 });
        std::cout << "Ethan Genser: Maze Generator in CPP (https://github.com/Ethan-Genser/Maze_Generator/tree/master)";

        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), COORD{ (short)OFFSET, (short)OFFSET + 11 });
        std::cout << "<- Press any key to go to main menu ....";

        _getch();
        this->menu();
    }

private:
    int row;
    int col;
    std::string name;
    int level;
    bool solved;
};

int main()
{
    std::srand(time(0));
    int size = 5 + rand() % (12 - 5);

    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), COORD{ (short)OFFSET, (short)OFFSET });

    std::cout << "Player: ";
    
    std::string name;
    std::cin >> name;

    Player player(0, 0, name, 1);
    player.menu();

    return 0;
}