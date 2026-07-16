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

    std::string machine;

    float duration = 0;

    float GetOutputPerMinute() const
    {
        if (duration <= 0 || products.empty())
            return 0;

        return (products[0].amount / duration) * 60.0f;
    }
    float GetCyclesPerMinute() const
    {
        if (duration <= 0)
            return 0;

        return 60.0f / duration;
    }

    std::vector<Ingredient> ingredients;
    std::vector<Product> products;
};