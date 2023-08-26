#ifndef PLAYER_H
#define PLAYER_H

#include <string>

class Player {
public:
    Player(int row, int col, std::string name, int level);
    void menu();
    void play();
    void scoreboard();
    void about();

private:
    int row;
    int col;
    std::string name;
    int level;
    bool solved;
};

#endif // !PLAYER_H
