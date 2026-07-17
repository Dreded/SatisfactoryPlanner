#include "Core/GameDatabase.h"

#include <iostream>
#include <fstream>
#include <algorithm>

#include <nlohmann/json.hpp>

using json = nlohmann::json;


std::string ExtractItemID(const std::string& input)
{
    auto pos = input.rfind('.');

    if (pos == std::string::npos)
        return "";

    std::string id = input.substr(pos + 1);


    // Remove Unreal path cleanup characters
    while (!id.empty() &&
        (id.back() == '\'' ||
            id.back() == '"' ||
            id.back() == ')'))
    {
        id.pop_back();
    }


    return id;
}

std::string ExtractMachineName(const std::string& input)
{
    if (input.find("AssemblerMk1") != std::string::npos)
        return "Assembler";

    if (input.find("Constructor") != std::string::npos)
        return "Constructor";

    if (input.find("Smelter") != std::string::npos)
        return "Smelter";

    if (input.find("Foundry") != std::string::npos)
        return "Foundry";

    if (input.find("Manufacturer") != std::string::npos)
        return "Manufacturer";

    if (input.find("Blender") != std::string::npos)
        return "Blender";

    if (input.find("Refinery") != std::string::npos)
        return "Refinery";

    if (input.find("Converter") != std::string::npos)
        return "Converter";

    return "Unknown";
}


std::vector<std::pair<std::string, int>> ParseItemList(const std::string& input)
{
    std::vector<std::pair<std::string, int>> result;

    size_t pos = 0;

    while (true)
    {
        auto itemStart = input.find("ItemClass=\"", pos);

        if (itemStart == std::string::npos)
            break;

        itemStart += 11;

        auto itemEnd = input.find("\"", itemStart);

        if (itemEnd == std::string::npos)
            break;

        std::string path = input.substr(
            itemStart,
            itemEnd - itemStart
        );


        auto amountStart = input.find("Amount=", itemEnd);

        if (amountStart == std::string::npos)
            break;

        amountStart += 7;

        auto amountEnd = input.find(")", amountStart);

        int amount = std::stoi(
            input.substr(
                amountStart,
                amountEnd - amountStart
            )
        );


        result.push_back(
            {
                ExtractItemID(path),
                amount
            }
        );


        pos = amountEnd;
    }

    return result;
}


bool GameDatabase::Load(const std::string& filename)
{
    std::ifstream file(filename);

    if (!file.is_open())
    {
        std::cerr << "Failed to open database file\n";
        return false;
    }


    try
    {
        json data;
        file >> data;

        std::cout << "Database loaded successfully\n";
        std::cout << "Top level entries: "
            << data.size()
            << "\n";


        if (!LoadItems(data))
            return false;


        if (!LoadRecipes(data))
            return false;


        BuildRecipeLookup();


        return true;
    }
    catch (const json::exception& e)
    {
        std::cerr << e.what() << "\n";
        return false;
    }
}



bool GameDatabase::LoadItems(const json& data)
{
    items.reserve(200);

    for (const auto& entry : data)
    {
        if (!entry.contains("NativeClass"))
            continue;


        std::string nativeClass = entry["NativeClass"];

if (nativeClass.find("FGItemDescriptor") == std::string::npos &&
    nativeClass.find("FGResourceDescriptor") == std::string::npos)
    continue;


        for (const auto& itemJson : entry["Classes"])
        {
            Item item;

            item.id = itemJson.value("ClassName", "");
            item.name = itemJson.value("mDisplayName", "");


            items.push_back(item);

            Item* ptr = &items.back();

            itemLookup[ptr->id] = ptr;
            itemNameLookup[ptr->name] = ptr;
        }
    }


    std::cout
        << "Loaded Items: "
        << items.size()
        << "\n";

    return true;
}



bool GameDatabase::LoadRecipes(const json& data)
{
    recipes.reserve(1000);

    for (const auto& entry : data)
    {
        if (!entry.contains("NativeClass"))
            continue;


        std::string nativeClass = entry["NativeClass"];


        if (nativeClass.find("FGRecipe") == std::string::npos)
            continue;


        for (const auto& recipeJson : entry["Classes"])
        {
            Recipe recipe;

            recipe.id = recipeJson.value("ClassName", "");
            recipe.name = recipeJson.value("mDisplayName", "");
            if (!recipeJson.contains("mIngredients") ||
                !recipeJson.contains("mProduct"))
            {
                continue;
            }
            recipe.machine =
                ExtractMachineName(
                    recipeJson.value("mProducedIn", "")
                );


            if (recipeJson.contains("mManufactoringDuration"))
            {
                recipe.duration =
                    std::stof(
                        recipeJson["mManufactoringDuration"].get<std::string>()
                    );
            }
            else
            {
                recipe.duration = 0;
            }


            if (recipeJson.contains("mIngredients"))
            {
                auto ingredients =
                    ParseItemList(
                        recipeJson["mIngredients"].get<std::string>()
                    );


                for (auto& ingredient : ingredients)
                {
                    auto found = itemLookup.find(ingredient.first);

                    if (found != itemLookup.end())
                    {
                        Ingredient item;

                        item.item = found->second;
                        item.amount = ingredient.second;

                        recipe.ingredients.push_back(item);
                    }
                }
            }


            if (recipeJson.contains("mProduct"))
            {
                auto products =
                    ParseItemList(
                        recipeJson["mProduct"].get<std::string>()
                    );


                for (auto& product : products)
                {
                    auto found = itemLookup.find(product.first);

                    if (found != itemLookup.end())
                    {
                        Product item;

                        item.item = found->second;
                        item.amount = product.second;

                        recipe.products.push_back(item);
                    }
                }
            }
            if (recipe.products.empty() ||
                recipe.ingredients.empty())
            {
                continue;
            }

            recipes.push_back(recipe);
        }

    }


    std::cout
        << "Loaded Recipes: "
        << recipes.size()
        << "\n";


    return true;
}



void GameDatabase::BuildRecipeLookup()
{
    for (auto& recipe : recipes)
    {
        for (auto& product : recipe.products)
        {
            recipeByProduct[product.item].push_back(&recipe);
        }
    }

}

Item* GameDatabase::FindItemByName(const std::string& name)
{
    auto it = itemNameLookup.find(name);

    if (it == itemNameLookup.end())
    {
        std::cerr << "Item not found: " << name << '\n';
        return nullptr;
    }
    return it->second;
}

Item* GameDatabase::FindItem(const std::string& id)
{
    auto it = itemLookup.find(id);

    if (it == itemLookup.end())
    {
        std::cerr << "Item ID not found: " << id << '\n';
        return nullptr;
    }
    return it->second;
}

Recipe* GameDatabase::FindRecipeByName(const std::string& name)
{
    for (auto& recipe : recipes)
    {
        if (recipe.name == name)
            return &recipe;
    }

    return nullptr;
}

Recipe* GameDatabase::GetDefaultRecipe(Item* item)
{
    auto found = recipeByProduct.find(item);

    if (found == recipeByProduct.end())
        return nullptr;

    for (auto* recipe : found->second)
    {
        if (recipe->machine == "Converter")
            continue;

        if (recipe->name.rfind("Alternate:", 0) == 0)
            continue;

        return recipe;
    }

    return nullptr;
}