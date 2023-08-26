#include "Cell.h"

void Cell::removeWalls(Cell current, Cell next) {
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