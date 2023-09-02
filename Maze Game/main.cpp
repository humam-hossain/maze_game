#include <iostream>
#include <cstdlib>
#include <ctime>
#include <Windows.h>
#include <string>

#include "Player.h"

#define OFFSET 5
#define ENTER_SOUND 988

int main()
{
    std::srand(time(0));

    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), COORD{ (short)OFFSET, (short)OFFSET });

    // Welcome Sound
    Beep(523, 500);

    std::cout << "Player: ";

    std::string name;
    std::cin >> name;

    Beep(ENTER_SOUND, 100);

    Player player(0, 0, name, 1);
    player.menu();

    return 0;
}