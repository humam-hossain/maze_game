#ifndef MAZE_H
#define MAZE_H

#include "Cell.h"
#include "Stack.h"
#include <string>

class Maze {
public:
    Maze(int);
    void setup();
    void setSize(int);
    Cell checkNeighbours(Cell);
    void generate();
    void draw(int, int, std::string, int, std::string, bool solved = false);
    bool isWall(int row, int col, int wallIndex);
    int getSize();

private:
    int size;
    Cell** grid;
    Stack stack;
    Cell current;
};

#endif // !MAZE_H
