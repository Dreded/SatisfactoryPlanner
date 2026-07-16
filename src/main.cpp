#include <iostream>

#include "Core/GameDatabase.h"
#include "Core/ProductionPlanner.h"


int main()
{
    std::cout << "Satisfactory Planner Starting...\n";


    GameDatabase database;

    if (!database.Load("data/en-US.json"))
    {
        std::cout << "Database load failed\n";
        return 1;
    }


    ProductionPlanner planner(database);
    Item* target =
        database.FindItemByName("Turbo Motor");


    if (target)
    {
        planner.Plan(
            target,
            1
        );
    }


    std::cout << "Done\n";

    return 0;
}