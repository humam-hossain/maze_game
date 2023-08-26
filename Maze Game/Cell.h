#ifndef CELL_H
#define CELL_H

class Cell {
public:
    int col;
    int row;
    Cell** parentGrid;
    int parentSize;

    bool visited = false;

    // top, right, bottom, left
    bool walls[4] = { true, true, true, true };

    void removeWalls(Cell current, Cell next);
};
#endif // !CELL_H