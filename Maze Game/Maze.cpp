#include "Maze.h"

#include <iostream>
#include <Windows.h>

#define OFFSET 5

Maze::Maze(int size) {
    this->size = size;

    this->grid = (Cell**)malloc(sizeof(Cell*) * size);

    for (int i = 0; i < size; ++i) {
        this->grid[i] = (Cell*)malloc(sizeof(Cell) * size);
    }
}

void Maze::setup() {
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

void Maze::setSize(int size) {
    free(this->grid);

    this->size = size;
    this->grid = (Cell**)malloc(sizeof(Cell*) * size);

    for (int i = 0; i < size; ++i) {
        this->grid[i] = (Cell*)malloc(sizeof(Cell) * size);
    }
}

Cell Maze::checkNeighbours(Cell cell) {
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

void Maze::generate() {
    this->grid[this->current.row][this->current.col].visited = true;

    if (checkNeighbours(this->current).col != -1) {
        Cell next = checkNeighbours(this->current);
        //std::cout << "(" << next.row << "," << next.col << ")\n";

        next.visited = true;
        this->stack.push(this->current);

        this->current.removeWalls(this->current, next);
        this->current = next;

        //std::cout << this->stack.getLength() << std::endl;
    }
    else if (this->stack.getLength() > 0) {
        this->current = this->stack.pop();
    }

    if (this->stack.getLength() == 0) {
        return;
    }

    this->generate();
}

void Maze::draw(int row, int col, std::string name, int level, std::string time, bool solved) {
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), COORD{ (short)OFFSET, (short)OFFSET - 4 });
    std::cout << "Level: " << level;

    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), COORD{ (short)OFFSET, (short)OFFSET - 3 });
    std::cout << "Player: " << name;

    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), COORD{ (short)OFFSET, (short)OFFSET - 2 });
    std::cout << "Duration: " << time;

    if (solved) {
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), COORD{ (short)(OFFSET + this->size * 2 - 4), (short)OFFSET - 1 });
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

    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), COORD{ (short)(OFFSET), (short)(OFFSET + this->size * 2 + 2) });
    std::cout << "<- BackSpace";
}

bool Maze::isWall(int row, int col, int wallIndex) {
    return this->grid[row][col].walls[wallIndex];
}

int Maze::getSize() {
    return this->size;
}