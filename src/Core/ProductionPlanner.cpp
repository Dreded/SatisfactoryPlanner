#include "Core/ProductionPlanner.h"

#include <iostream>


ProductionPlanner::ProductionPlanner(GameDatabase& database)
    :
    database(database)
{

}


void ProductionPlanner::Plan(Item* target, float amount)
{
    auto tree = Resolve(
        target,
        amount
    );


    std::cout << "\nProduction Plan:\n";

    PrintNode(tree);


    baseResources.clear();

    CollectBaseResources(tree);


    std::cout << "\nBase Resources:\n";

    for (auto& item : baseResources)
    {
        std::cout
            << "  "
            << item.first->name
            << " x"
            << item.second
            << "\n";
    }
}

void ProductionPlanner::SetRecipe(Item* item, Recipe* recipe)
{
    if (!item)
    {
        std::cerr << "SetRecipe(): item is nullptr.\n";
        return;
    }

    if (!recipe)
    {
        std::cerr << "SetRecipe(): recipe is nullptr.\n";
        return;
    }

    selectedRecipes[item] = recipe;
}


ProductionNode ProductionPlanner::Resolve(
    Item* item,
    float rate
)
{
    ProductionNode node;

    node.item = item;
    node.rate = rate;


    auto recipes =
        database.GetRecipeLookup().find(item);


    if (recipes == database.GetRecipeLookup().end())
    {
        baseResources[item] += rate;
        return node;
    }


    Recipe* recipe = nullptr;


    auto selected = selectedRecipes.find(item);

    if (selected != selectedRecipes.end())
    {
        recipe = selected->second;
    }
    else
    {
        recipe = database.GetDefaultRecipe(item);
    }


    if (!recipe)
    {
        baseResources[item] += rate;
        return node;
    }


    node.recipe = recipe;
    node.machineRate =
        recipe->GetOutputPerMinute();

    node.machinesRequired =
        rate / node.machineRate;

    float multiplier =
        rate /
        node.machineRate;


    for (auto& ingredient : recipe->ingredients)
    {
        node.children.push_back(
            Resolve(
                ingredient.item,
                ingredient.amount *
                recipe->GetCyclesPerMinute() *
                node.machinesRequired
            )
        );
    }


    return node;
}

void ProductionPlanner::PrintNode(
    const ProductionNode& node,
    int depth
)
{
    std::cout
        << std::string(depth * 2, ' ')
        << node.item->name
        << " x"
        << node.rate;


    if (node.recipe)
    {
        std::cout
            << " ["
            << node.recipe->machine
            << "]";

        std::cout
            << " "
            << node.recipe->GetOutputPerMinute()
            << "/min";
    }


    std::cout << "\n";


    for (const auto& child : node.children)
    {
        PrintNode(child, depth + 1);
    }
}

void ProductionPlanner::CollectBaseResources(
    const ProductionNode& node
)
{
    if (node.children.empty())
    {
        baseResources[node.item] += node.rate;
        return;
    }


    for (const auto& child : node.children)
    {
        CollectBaseResources(child);
    }
}
