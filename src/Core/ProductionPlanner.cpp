#include "Core/ProductionPlanner.h"

#include <iostream>


ProductionPlanner::ProductionPlanner(GameDatabase& database)
    :
    database(database)
{

}


void ProductionPlanner::Plan(Item* target, float amount)
{
    requiredItems.clear();


    std::cout
        << "\nProduction Plan:\n";


    Resolve(
        target,
        amount,
        0
    );


    std::cout
        << "\nRaw Materials:\n";


    for (auto& item : requiredItems)
    {
        std::cout
            << "  "
            << item.first->name
            << " x"
            << item.second
            << "\n";
    }
}



void ProductionPlanner::Resolve(
    Item* item,
    float amount,
    int depth
)
{
    auto recipes =
        database.GetRecipeLookup().find(item);


    // Raw resource
    if (recipes == database.GetRecipeLookup().end() ||
        recipes->second.front()->ingredients.empty())
    {
        requiredItems[item] += amount;
        return;
    }


    Recipe* recipe =
        recipes->second.front();


    float multiplier =
        amount / recipe->products[0].amount;


    std::cout
        << std::string(depth * 2, ' ')
        << item->name
        << " x"
        << amount
        << "\n";


    for (auto& ingredient : recipe->ingredients)
    {
        Resolve(
            ingredient.item,
            ingredient.amount * multiplier,
            depth + 1
        );
    }
}