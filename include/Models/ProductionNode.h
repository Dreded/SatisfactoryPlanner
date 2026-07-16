#pragma once

#include <vector>

#include "Models/Item.h"
#include "Models/Recipe.h"


struct ProductionNode
{
    Item* item = nullptr;

    float rate = 0;              // required items/minute

    Recipe* recipe = nullptr;

    float machineRate = 0;       // recipe output/minute

    float machinesRequired = 0;

    std::vector<ProductionNode> children;
};