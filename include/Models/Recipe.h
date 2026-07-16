#pragma once

#include <string>
#include <vector>

#include "Models/Item.h"


struct Ingredient
{
    Item* item;
    float amount;
};


struct Product
{
    Item* item;
    float amount;
};


struct Recipe
{
    std::string id;
    std::string name;

    float duration = 0;

    std::vector<Ingredient> ingredients;
    std::vector<Product> products;
};