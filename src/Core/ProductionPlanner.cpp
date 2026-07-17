#include <iostream>
#include <cmath>

#include "Core/ProductionPlanner.h"
#include "Core/FactoryPrinter.h"


ProductionPlanner::ProductionPlanner(GameDatabase& database)
    :
    database(database)
{

}


void ProductionPlanner::Plan(Item* target, float amount)
{
    targetItem = target;

    auto tree = Resolve(
        target,
        amount
    );


    baseResources.clear();
    machineRequirements.clear();
    machines.clear();
    connections.clear();


    CollectBaseResources(tree);
    CollectMachines(tree);
    BuildMachineList();
    BuildConnections();
    ValidateConnections();

    auto graph = GetFactoryGraph();

    FactoryPrinter::PrintFactoryGraph(graph, target);

    //FactoryPrinter::PrintResources(graph);
    //FactoryPrinter::PrintMachineRequirements(graph);

    //FactoryPrinter::PrintMachines(graph);
    //FactoryPrinter::PrintMachineDetails(graph);
    //FactoryPrinter::PrintMachineGroups(graph);
    //FactoryPrinter::PrintConnections(graph);
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

void ProductionPlanner::BuildConnections()
{
    struct InputDemand
    {
        ProductionMachine* machine;
        Item* item;
        float remaining;
    };


    std::vector<InputDemand> demands;


    // Build a list of all required inputs
    for (auto& machine : machines)
    {
        for (auto& input : machine.inputs)
        {
            InputDemand demand;

            demand.machine = &machine;
            demand.item = input.item;
            demand.remaining = input.rate;

            demands.push_back(demand);
        }
    }


    // Match outputs to inputs
    for (auto& producer : machines)
    {
        for (auto& output : producer.outputs)
        {
            float remainingOutput = output.rate;


            for (auto& demand : demands)
            {
                if (remainingOutput <= 0)
                    break;


                if (demand.item != output.item)
                    continue;


                if (demand.remaining <= 0)
                    continue;


                float amount =
                    std::min(
                        remainingOutput,
                        demand.remaining
                    );


                MachineConnection connection;

                connection.from = &producer;
                connection.to = demand.machine;
                connection.item = output.item;
                connection.rate = amount;


                connections.push_back(connection);


                remainingOutput -= amount;
                demand.remaining -= amount;
            }
        }
    }
}

void ProductionPlanner::ValidateConnections()
{
    std::cout << "\nConnection Validation:\n";


    for (auto& machine : machines)
    {
        std::cout
            << "#"
            << machine.id
            << " "
            << machine.recipe->machine
            << " - "
            << machine.recipe->name
            << "\n";


        // Check outputs
        for (auto& output : machine.outputs)
        {
            float connected = 0;


            for (auto& connection : connections)
            {
                if (connection.from == &machine &&
                    connection.item == output.item)
                {
                    connected += connection.rate;
                }
            }


            float difference =
                output.rate - connected;


            std::cout
                << "  Output "
                << output.item->name
                << ": "
                << connected
                << "/"
                << output.rate;


            if (std::abs(difference) < 0.01f)
            {
                std::cout << " OK";
            }
            else if (output.item == targetItem)
            {
                std::cout << " WARNING (No destination)";
            }
            else
            {
                std::cout
                    << " ERROR ("
                    << difference
                    << " remaining)";
            }


            std::cout << "\n";
        }


        // Check inputs
        for (auto& input : machine.inputs)
        {
            float connected = 0;


            for (auto& connection : connections)
            {
                if (connection.to == &machine &&
                    connection.item == input.item)
                {
                    connected += connection.rate;
                }
            }


            float difference =
                input.rate - connected;


            std::cout
                << "  Input "
                << input.item->name
                << ": "
                << connected
                << "/"
                << input.rate;


            bool isRawResource =
                baseResources.find(input.item) != baseResources.end();


            if (std::abs(difference) < 0.01f)
            {
                std::cout << " OK";
            }
            else if (isRawResource)
            {
                std::cout << " WARNING (Resource source required)";
            }
            else
            {
                std::cout
                    << " ERROR ("
                    << difference
                    << " missing)";
            }


            std::cout << "\n";
        }
    }
}

FactoryGraph ProductionPlanner::GetFactoryGraph()
{
    FactoryGraph graph;


    for (auto& machine : machines)
    {
        graph.machines.push_back(&machine);
    }


    for (auto& connection : connections)
    {
        graph.connections.push_back(&connection);
    }


    graph.resources = baseResources;

    graph.machineRequirements = machineRequirements;


    return graph;
}