#pragma once

#include "Core/GameDatabase.h"

#include <unordered_map>
#include <string>


class ProductionPlanner
{
public:

    ProductionPlanner(GameDatabase& database);

    void Plan(Item* target, float amount);
    void SetRecipe(Item* item, Recipe* recipe);

private:

    GameDatabase& database;


    void Resolve(
        Item* item,
        float amount,
        int depth
    );


    std::unordered_map<Item*, float> requiredItems;
    std::unordered_map<Item*, Recipe*> selectedRecipes;
};