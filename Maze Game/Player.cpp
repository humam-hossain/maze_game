#include "Player.h"
#include "Maze.h"

#include <Windows.h>
#include <iostream>
#include <sstream>
#include <conio.h>
#include <chrono>
#include <fstream>
#include <thread>
#include <vector>
#include <algorithm>


#define OFFSET 5

// keys
#define ARROW_KEY 224
#define ARROW_UP 72
#define ARROW_RIGHT 77
#define ARROW_DOWN 80
#define ARROW_LEFT 75
#define ENTER_KEY 13
#define BACKSPACE_KEY 8

// sounds
#define SELECT_SOUND 262
#define ENTER_SOUND 988
#define MOVE_SOUND 500
#define EXIT_SOUND 300
#define SOLVED_SOUND 523

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
                Beep(SELECT_SOUND, 20);
                clearScreen();
            }
            else if (key == ARROW_DOWN && pos != 3) {
                pos++;
                Beep(SELECT_SOUND, 20);
                clearScreen();
            }
        }
        else if (key == ENTER_KEY) {
            switch (pos)
            {
            case 0: // play
                Beep(784, 150);
                Beep(988, 150);
                Beep(1319, 300);
                this->play();
                break;
            case 1: // scoreboard
                Beep(ENTER_SOUND, 150);
                this->scoreboard();
                break;
            case 2: // about
                Beep(ENTER_SOUND, 150);
                this->about();
                break;
            case 3: // exit
                break;
            }

            SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), COORD{ (short)0, (short)OFFSET + 8 });
            Beep(EXIT_SOUND, 300);
            std::cout << "Closing...";
            return;
        }
    }

}

void Player::play() {
    clearScreen();
    std::string time = "00:00:00";
    int64_t minutes, seconds, milliseconds;

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
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);

            minutes = std::chrono::duration_cast<std::chrono::minutes>(duration).count();
            duration -= std::chrono::minutes(minutes);
            seconds = std::chrono::duration_cast<std::chrono::seconds>(duration).count();
            duration -= std::chrono::seconds(seconds);
            milliseconds = duration.count();

            std::string min = std::to_string(minutes);
            std::string sec = std::to_string(seconds);
            std::string milli = std::to_string(milliseconds);

            if (min.length() == 1) {
                min = "0" + min;
            }

            if (sec.length() == 1) {
                sec = "0" + sec;
            }

            time = min + ":" + sec + ":" + milli;

            // player move
            if (key == ARROW_UP && !maze->isWall(this->row, this->col, 0)) {
                this->row--;
                Beep(MOVE_SOUND, 10);
                clearScreen();
                maze->draw(this->row, this->col, this->name, this->level, time);
            }
            else if (key == ARROW_RIGHT && !maze->isWall(this->row, this->col, 1)) {
                this->col++;
                Beep(MOVE_SOUND, 10);
                clearScreen();
                maze->draw(this->row, this->col, this->name, this->level, time);
            }
            else if (key == ARROW_DOWN && !maze->isWall(this->row, this->col, 2)) {
                this->row++;
                Beep(MOVE_SOUND, 10);
                clearScreen();
                maze->draw(this->row, this->col, this->name, this->level, time);
            }
            else if (key == ARROW_LEFT && !maze->isWall(this->row, this->col, 3)) {
                this->col--;
                Beep(MOVE_SOUND, 10);
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

            file << this->name << "," << this->level << "," << minutes  << "," << seconds << "," << milliseconds << "\n";

            file.close();

            // show
            clearScreen();
            maze->draw(this->row, this->col, this->name, this->level, time, this->solved);
            Beep(523, 300);
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
            Beep(EXIT_SOUND, 300);
            break;
        }

    }

    delete maze;
    this->menu();
}

bool sort_scores(const std::vector<std::string>& a, const std::vector<std::string>& b) {
    int aLevel = std::stoi(a[1]);
    int bLevel = std::stoi(b[1]);

    if (aLevel > bLevel) {
        return true;
    }
    else if (aLevel < bLevel) {
        return false;
    }

    for (int i = 2; i <= 4; ++i) {
        int aValue = std::stoi(a[i]);
        int bValue = std::stoi(b[i]);

        if (aValue < bValue) {
            return true;
        }
        else if (aValue > bValue) {
            return false;
        }
    }

    return false;
}

void Player::scoreboard() {
    // scores
    std::ifstream file("scores.txt");

    if (!file) {
        std::cerr << "ERROR: opening scores.txt\n";
        return;
    }

    std::string line;
    std::vector<std::vector<std::string>> scores;

    while (std::getline(file, line)) {
        std::istringstream stream(line);
        std::string token;
        std::vector<std::string> tokens;
        char delimiter = ',';

        while (std::getline(stream, token, delimiter)) {
            tokens.push_back(token);
        }

        scores.push_back(tokens);
    }
    
    file.close();
    std::sort(scores.begin(), scores.end(), sort_scores);

    // print
    int key;
    int start = 0;

    while (true) {
        clearScreen();
        // header
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), COORD{ (short)OFFSET, (short)OFFSET });
        std::cout << "SCOREBOARD";
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), COORD{ (short)OFFSET, (short)OFFSET + 1 });
        std::cout << "--------------------";

        // scores
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), COORD{ (short)OFFSET, (short)(OFFSET + 3) });
        std::cout << "\tNAME\tLEVEL\tDURATION";

        size_t size = scores.size() > (5 + start) ? (5 + start) : scores.size();

        for (size_t i = start; i < size; i++) {
            SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), COORD{ (short)OFFSET, (short)(OFFSET + 4 + i - start) });
            std::cout << (i + 1) << "\t" << scores[i][0] << "\t" << scores[i][1] << "\t" << scores[i][2] << ":" << scores[i][3] << ":" << scores[i][4];
        }

        // <- back
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), COORD{ (short)OFFSET, (short)OFFSET + 11 });
        std::cout << "<- Press any key to go to main menu ....";

        key = _getch();
        if (key == ARROW_KEY) {
            key = _getch();

            if (key == ARROW_UP && start != 0) {
                Beep(500, 10);
                start--;
            }
            else if (key == ARROW_DOWN && start != scores.size() - 5) {
                Beep(500, 10);
                start++;
            }
        }
        else {
            break;
        }
    }

    Beep(EXIT_SOUND, 300);
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

    int key = _getch();
    Beep(EXIT_SOUND, 300);
    this->menu();
}