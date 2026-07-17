#include "Core/ProductionPlanner.h"

#include <iostream>
#include <cmath>


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


    baseResources.clear();
    machineRequirements.clear();
    machines.clear();


    CollectBaseResources(tree);
    CollectMachines(tree);
    BuildMachineList();


    std::cout << "\nProduction Plan:\n";
    PrintNode(tree);


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


    std::cout << "\nMachines Required:\n";

    for (auto& machine : machineRequirements)
    {
        std::cout
            << "  "
            << machine.first->machine
            << ": "
            << machine.first->name
            << " x"
            << machine.second
            << "\n";
    }


    PrintMachines();
    PrintMachineDetails();
    PrintGroupedMachines();
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
        return node;
    }


    node.recipe = recipe;
    node.machineRate =
        recipe->GetOutputPerMinute();

    node.machinesRequired =
        rate / node.machineRate;

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

void ProductionPlanner::PrintMachines()
{
    std::cout << "\nMachine List:\n";

    int count = 1;

    for (const auto& machine : machines)
    {
        std::cout
            << "#"
            << machine.id
            << " "
            << machine.recipe->machine
            << " - "
            << machine.recipe->name
            << " @ "
            << machine.clockSpeed * 100
            << "%\n";
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

void ProductionPlanner::CollectMachines(const ProductionNode& node)
{
    if (node.recipe)
    {
        machineRequirements[node.recipe] +=
            node.machinesRequired;
    }

    for (const auto& child : node.children)
    {
        CollectMachines(child);
    }
}

void ProductionPlanner::BuildMachineList()
{
    int id = 1;

    for (auto& requirement : machineRequirements)
    {
        Recipe* recipe = requirement.first;
        float count = requirement.second;

        int wholeMachines =
            static_cast<int>(std::ceil(count));


        for (int i = 0; i < wholeMachines; i++)
        {
            ProductionMachine machine;

            machine.id = id++;

            machine.recipe = recipe;

            machine.clockSpeed =
                count / wholeMachines;


            // Calculate outputs
            for (auto& product : recipe->products)
            {
                ItemRate output;

                output.item = product.item;

                output.rate =
                    recipe->GetOutputPerMinute()
                    * machine.clockSpeed;

                machine.outputs.push_back(output);
            }


            // Calculate inputs
            for (auto& ingredient : recipe->ingredients)
            {
                ItemRate input;

                input.item = ingredient.item;

                input.rate =
                    ingredient.amount
                    *
                    recipe->GetCyclesPerMinute()
                    *
                    machine.clockSpeed;

                machine.inputs.push_back(input);
            }

            machines.push_back(machine);
        }
    }
}
void ProductionPlanner::PrintMachineDetails()
{
    std::cout << "\nMachine Details:\n";

    for (auto& machine : machines)
    {
        std::cout
            << "#"
            << machine.id
            << " "
            << machine.recipe->machine
            << " - "
            << machine.recipe->name
            << " @ "
            << machine.clockSpeed * 100
            << "%\n";


        std::cout << "  Inputs:\n";

        if (machine.inputs.empty())
        {
            std::cout << "    None\n";
        }
        else
        {
            for (auto& input : machine.inputs)
            {
                std::cout
                    << "    "
                    << input.item->name
                    << " "
                    << input.rate
                    << "/min\n";
            }
        }


        std::cout << "  Outputs:\n";

        if (machine.outputs.empty())
        {
            std::cout << "    None\n";
        }
        else
        {
            for (auto& output : machine.outputs)
            {
                std::cout
                    << "    "
                    << output.item->name
                    << " "
                    << output.rate
                    << "/min\n";
            }
        }


        std::cout << "\n";
    }
}
void ProductionPlanner::PrintGroupedMachines()
{
    struct MachineGroup
    {
        Recipe* recipe = nullptr;
        float clockSpeed = 0;
        std::vector<int> ids;
    };


    std::vector<MachineGroup> groups;


    for (const auto& machine : machines)
    {
        bool found = false;

        for (auto& group : groups)
        {
            if (group.recipe == machine.recipe &&
                group.clockSpeed == machine.clockSpeed)
            {
                group.ids.push_back(machine.id);
                found = true;
                break;
            }
        }


        if (!found)
        {
            MachineGroup group;

            group.recipe = machine.recipe;
            group.clockSpeed = machine.clockSpeed;
            group.ids.push_back(machine.id);

            groups.push_back(group);
        }
    }


    std::cout << "\nGrouped Machines:\n";


    for (const auto& group : groups)
    {
        std::cout
            << "  "
            << group.ids.size()
            << "x "
            << group.recipe->machine
            << " - "
            << group.recipe->name
            << " @ "
            << group.clockSpeed * 100
            << "%\n";


        std::cout << "    IDs: ";

        for (size_t i = 0; i < group.ids.size(); i++)
        {
            std::cout
                << "#"
                << group.ids[i];

            if (i < group.ids.size() - 1)
                std::cout << ", ";
        }

        std::cout << "\n";
    }
}