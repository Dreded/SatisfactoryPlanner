#pragma once

#include <vector>

#include "Models/Recipe.h"
#include "Models/ItemRate.h"


struct ProductionMachine
{
    int id = 0;
    Recipe* recipe = nullptr;

    float clockSpeed = 1.0f;

    std::vector<ItemRate> inputs;
    std::vector<ItemRate> outputs;
};