#pragma once

#include <string>
#include <vector>

#include "Models/Recipe.h"


class RecipeParser
{
public:

    static std::vector<Ingredient> ParseIngredients(
        const std::string& data
    );

    static std::vector<Product> ParseProducts(
        const std::string& data
    );
};