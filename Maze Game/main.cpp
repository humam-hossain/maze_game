#include <iostream>
#include <cstdlib>
#include <ctime>
#include <Windows.h>
#include <chrono>

class Cell {
public:
    int col;
    int row;
    Cell** parentGrid;
    int parentSize;

    bool visited = false;

    // top, right, botto, left
    bool walls[4] = {true, true, true, true};

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

        this->generate();
    }

    void draw() {
        for (int i = 0; i < this->size; ++i) {
            for (int j = 0; j < this->size; ++j) {
                int left = j * 4;
                int top = i * 2;

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
            }
        }

        std::cout << "\n\n\n";
    }

private:
    int size;
    Cell** grid;
    Stack stack;
};

int main()
{
    std::srand(time(0));
    int size = 10;

    Maze maze(size);
    maze.setup();
    maze.generate();
    maze.draw();

    return 0;
}