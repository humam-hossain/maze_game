#include "Player.h"
#include "Maze.h"

#include <Windows.h>
#include <iostream>
#include <conio.h>
#include <chrono>
#include <fstream>
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

Player::Player(int row, int col, std::string name, int level) {
    this->row = row;
    this->col = col;
    this->name = name;
    this->level = level;
    this->solved = false;
}

void Player::menu() {
    clearScreen();

    int pos = 0;
    int key;

    while (true) {
        // header
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), COORD{ (short)OFFSET + 4, (short)OFFSET });
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
            else if (key == ARROW_DOWN && pos != 3) {
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
                this->scoreboard();
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

void Player::play() {
    std::string time = "00:00:00";
    clearScreen();

    Maze* maze = new Maze(this->level + 5);
    maze->setup();
    maze->generate();
    maze->draw(this->row, this->col, this->name, this->level, time);

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
            if (key == ARROW_UP && !maze->isWall(this->row, this->col, 0)) {
                this->row--;
                clearScreen();
                maze->draw(this->row, this->col, this->name, this->level, time);
            }
            else if (key == ARROW_RIGHT && !maze->isWall(this->row, this->col, 1)) {
                this->col++;
                clearScreen();
                maze->draw(this->row, this->col, this->name, this->level, time);
            }
            else if (key == ARROW_DOWN && !maze->isWall(this->row, this->col, 2)) {
                this->row++;
                clearScreen();
                maze->draw(this->row, this->col, this->name, this->level, time);
            }
            else if (key == ARROW_LEFT && !maze->isWall(this->row, this->col, 3)) {
                this->col--;
                clearScreen();
                maze->draw(this->row, this->col, this->name, this->level, time);
            }
        }

        // check goal reached
        if (this->row == maze->getSize() - 1 && this->col == maze->getSize() - 1) {
            this->solved = true;

            // save score
            std::ofstream file("scores.txt", std::ios::app);

            if (!file) {
                std::cerr << "ERROR: opening scores.txt\n";
                return;
            }

            file << this->name << "," << this->level << "," << time << "\n";

            file.close();

            // show
            clearScreen();
            maze->draw(this->row, this->col, this->name, this->level, time, this->solved);
            std::this_thread::sleep_for(std::chrono::seconds(1));

            // increase level
            this->level++;

            // reset for new level
            this->solved = false;
            time = "00:00:00";
            this->row = 0;
            this->col = 0;

            // new level maze
            maze->setSize(this->level + 5);
            maze->setup();
            maze->generate();

            clearScreen();
            maze->draw(this->row, this->col, this->name, this->level, time);
            startTime = std::chrono::high_resolution_clock::now();
        }

        // <- back
        if (key == BACKSPACE_KEY) {
            break;
        }

    }

    delete maze;
    this->menu();
}

void Player::scoreboard() {
    clearScreen();

    // header
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), COORD{ (short)OFFSET, (short)OFFSET });
    std::cout << "SCOREBOARD";
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), COORD{ (short)OFFSET, (short)OFFSET + 1 });
    std::cout << "--------------------";

    // scores
    std::ifstream file("scores.txt");

    if (!file) {
        std::cerr << "ERROR: opening scores.txt\n";
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::cout << line << "\n";
    }

    file.close();

    // <- back
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), COORD{ (short)OFFSET, (short)OFFSET + 11 });
    std::cout << "<- Press any key to go to main menu ....";

    _getch();
    this->menu();
}

void Player::about() {
    clearScreen();

    // header
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), COORD{ (short)OFFSET, (short)OFFSET });
    std::cout << "ABOUT";
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), COORD{ (short)OFFSET, (short)OFFSET + 1 });
    std::cout << "--------------------";

    // intro
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), COORD{ (short)OFFSET, (short)OFFSET + 2 });
    std::cout << "Experience a captivating maze game powered by the Depth-First Search (DFS) recursive backtracking algorithm.";
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), COORD{ (short)OFFSET, (short)OFFSET + 3 });
    std::cout << "Traverse through a dynamically generated maze, navigating pathways and walls using arrow keys.This game not";
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), COORD{ (short)OFFSET, (short)OFFSET + 4 });
    std::cout << "only provides entertainment but also offers insights into algorithms, problem - solving, and C++ coding. Embark";
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), COORD{ (short)OFFSET, (short)OFFSET + 5 });
    std::cout << "on a journey of exploration and learn while having fun.";

    // references
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), COORD{ (short)OFFSET, (short)OFFSET + 7 });
    std::cout << "References:";
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), COORD{ (short)OFFSET, (short)OFFSET + 8 });
    std::cout << "Wikipedia: Maze generation algorithm (https://en.wikipedia.org/wiki/Maze_generation_algorithm)";
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), COORD{ (short)OFFSET, (short)OFFSET + 9 });
    std::cout << "Ethan Genser: Maze Generator in CPP (https://github.com/Ethan-Genser/Maze_Generator/tree/master)";

    // <- back
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), COORD{ (short)OFFSET, (short)OFFSET + 11 });
    std::cout << "<- Press any key to go to main menu ....";

    _getch();
    this->menu();
}