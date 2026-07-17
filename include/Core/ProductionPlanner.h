#pragma once

#include <unordered_map>
#include <string>
#include <vector>

#include "Core/GameDatabase.h"
#include "Models/ProductionNode.h"
#include "Models/ProductionMachine.h"
#include "Models/MachineConnection.h"
#include "Models/FactoryGraph.h"

class ProductionPlanner
{
public:

    ProductionPlanner(GameDatabase& database);

    void Plan(Item* target, float amount);
    void SetRecipe(Item* item, Recipe* recipe);

private:

    ProductionNode Resolve(
        Item* item,
        float amount
    );

    FactoryGraph GetFactoryGraph();

    void CollectBaseResources( const ProductionNode& node);
    void CollectMachines( const ProductionNode& node);
    void BuildMachineList();
    void BuildConnections();
    void ValidateConnections();

    GameDatabase& database;
    Item* targetItem = nullptr;

    std::unordered_map<Item*, float> baseResources;
    std::unordered_map<Recipe*, float> machineRequirements;

    std::unordered_map<Item*, Recipe*> selectedRecipes;

    std::vector<ProductionMachine> machines;
    std::vector<MachineConnection> connections;
};