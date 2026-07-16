#include <iostream>

#include "Core/GameDatabase.h"


int main()
{
    std::cout << "Satisfactory Planner Starting..."
        << std::endl;


    GameDatabase database;

    database.Load("data/game.json");


    return 0;
}