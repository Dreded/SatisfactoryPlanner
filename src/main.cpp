#include <iostream>

#include "Core/GameDatabase.h"


int main()
{
    std::cout << "Satisfactory Planner Starting...\n";


    GameDatabase database;

    if (!database.Load("data/en-US.json"))
    {
        std::cout << "Database load failed\n";
        return 1;
    }


    std::cout << "Done\n";

    return 0;
}