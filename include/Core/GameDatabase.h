#pragma once

#include <string>
#include <vector>
#include <unordered_map>

#include "Models/Recipe.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;


class GameDatabase
{
public:

    bool Load(const std::string& filename);

    Item* FindItemByName(const std::string& name);
    Item* FindItem(const std::string& id);

    Recipe* FindRecipeByName(const std::string& name);
    Recipe* GetDefaultRecipe(Item* item);

    const std::vector<Recipe>& GetRecipes() const
    {
        return recipes;
    }

    const std::vector<Item>& GetItems() const
    {
        return items;
    }

    const std::vector<Recipe*>& GetRecipesForItem(Item* item) const
    {
        auto it = recipeByProduct.find(item);

        if (it != recipeByProduct.end())
        {
            return it->second;
        }

        static const std::vector<Recipe*> empty;
        return empty;
    }

    const auto& GetRecipeLookup() const
    {
        return recipeByProduct;
    }

private:

    bool LoadItems(const nlohmann::json& data);
    bool LoadRecipes(const nlohmann::json& data);
    void BuildRecipeLookup();


    std::vector<Recipe> recipes;
    std::vector<Item> items;

    std::unordered_map<std::string, Item*> itemLookup;
    std::unordered_map<std::string, Item*> itemNameLookup;
    std::unordered_map<Item*, std::vector<Recipe*>> recipeByProduct;
};