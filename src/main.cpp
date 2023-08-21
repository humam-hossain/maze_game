#include <iostream>
#include <time.h>

struct Cell {
    int col;
    int row;
    Cell **parentGrid;
    int parentSize;
    
    bool visited = false;
};

Cell current;

class Maze {
public:
    Maze(int size) {
        this->size = size;

        this->grid = (Cell **)malloc(sizeof(Cell *) * size);

        for(int i=0; i<size; ++i){
            this->grid[i] = (Cell *)malloc(sizeof(Cell) * size);
        }
    }

    void setup() {
        for(int i=0; i<this->size; ++i){
            for(int j=0; j<this->size; ++j){
                struct Cell cell;

                cell.row = i;
                cell.col = j;
                cell.parentGrid = this->grid;
                cell.parentSize = this->size;
                
                this->grid[i][j] = cell;
            }
        }

        current = this->grid[0][0];
    }

    Cell checkNeighbours(Cell cell){
        int row = cell.row;
        int col = cell.col;

        int count = 0;

        Cell *arr = (Cell *)malloc(sizeof(Cell) * 4);

        if(row != 0 && this->grid[row - 1][col].visited == false){
            arr[count] = this->grid[row - 1][col];
            count++;
        }

        if(col != this->size - 1 && this->grid[row][col + 1].visited == false){
            arr[count] = this->grid[row][col + 1];
            count++;
        }

        if(row != this->size-1 && this->grid[row + 1][col].visited == false){
            arr[count] = this->grid[row + 1][col];
            count++;
        }

        if(col != 0 && this->grid[row][col - 1].visited == false){
            arr[count] = this->grid[row][col - 1];
            count++;
        }

        if(count != 0){
            // random neighbour
            srand(time(0));
            int i = rand() % count;
            // std::cout << "(" << arr[i].col << "," << arr[i].row << ")\n";
            return arr[i];
        }

        struct Cell null;
        null.col = -1;
        null.row = -1;

        return null;
    }

    void draw(){
        this->grid[current.row][current.col].visited = true;

        for(int i=0; i<this->size; ++i){
            for(int j=0; j<this->size; ++j){
                char value = this->grid[i][j].visited ? ' ' : '#';
                std::cout << value;
            }
            std::cout << "\n";
        }

        if(checkNeighbours(current).col != -1){
            struct Cell next = checkNeighbours(current);
            std::cout << "(" << next.row << "," << next.col << ")\n";
        }
    }

private:
    int size;
    Cell **grid;
    Cell *stack;
};

int main()
{
    int size = 20;

    Maze maze(size);
    maze.setup();
    maze.draw();

    std::cout << "current = (" << current.col << "," << current.row << ")\n";
    return 0;
}