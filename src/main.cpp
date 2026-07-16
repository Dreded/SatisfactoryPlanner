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

    auto* target = database.FindItemByName("Turbo Motor");

    if (!target)
    {
        std::cerr << "Target item not found.\n";
        return 1;
    }

    auto* ironPlate = database.FindItemByName("Iron Plate");

    if (!ironPlate)
    {
        std::cerr << "Item not found: Iron Plate\n";
        return 1;
    }

    auto* steelCastPlate =
        database.FindRecipeByName("Alternate: Steel Cast Plate");

    if (!steelCastPlate)
    {
        std::cerr << "Recipe not found: Alternate: Steel Cast Plate\n";
        return 1;
    }

    planner.SetRecipe(
        database.FindItemByName("Iron Plate"),
        database.FindRecipeByName("Alternate: Steel Cast Plate")
    );

    planner.Plan(target, 1);

    std::cout << "Done\n";

    return 0;
}