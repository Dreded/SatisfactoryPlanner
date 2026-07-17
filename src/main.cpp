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

    // auto* target = database.FindItemByName("Versatile Framework");

    auto* target = database.FindItemByName("Reinforced Iron Plate");

    if (!target)
    {
        std::cerr << "Target item not found.\n";
        return 1;
    }


    planner.Plan(target, 10);

    std::cout << "Done\n";

    return 0;
}
