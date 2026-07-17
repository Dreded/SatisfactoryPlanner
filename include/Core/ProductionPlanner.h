#pragma once

#include <unordered_map>
#include <string>

#include "Core/GameDatabase.h"
#include "Models/ProductionNode.h"


class ProductionPlanner
{
public:

    ProductionPlanner(GameDatabase& database);

    void Plan(Item* target, float amount);
    void SetRecipe(Item* item, Recipe* recipe);
    void CollectMachines(const ProductionNode& node);

private:

    ProductionNode Resolve(
        Item* item,
        float amount
    );

    struct MachineRequirement
    {
        Recipe* recipe = nullptr;
        float machines = 0;
    };

    void PrintNode(const ProductionNode& node, int depth = 0);

    void CollectBaseResources(
        const ProductionNode& node
    );

    GameDatabase& database;

    std::unordered_map<Item*, float> baseResources;
    std::unordered_map<Item*, Recipe*> selectedRecipes;
    std::unordered_map<Recipe*, float> machineRequirements;
};