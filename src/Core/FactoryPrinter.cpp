#include "Core/FactoryPrinter.h"

#include <iostream>
#include <map>
#include <iomanip>


void FactoryPrinter::PrintMachines(
    const FactoryGraph& graph
)
{
    std::cout << "\nMachine List:\n";

    for (const auto* machine : graph.machines)
    {
        std::cout
            << "#"
            << machine->id
            << " "
            << machine->recipe->machine
            << " - "
            << machine->recipe->name
            << " @ "
            << machine->clockSpeed * 100
            << "%\n";
    }
}


void FactoryPrinter::PrintMachineDetails(
    const FactoryGraph& graph
)
{
    std::cout << "\nMachine Details:\n";

    for (const auto* machine : graph.machines)
    {
        std::cout
            << "#"
            << machine->id
            << " "
            << machine->recipe->machine
            << " - "
            << machine->recipe->name
            << "\n";

        std::cout << " Inputs:\n";

        for (auto& input : machine->inputs)
        {
            std::cout
                << "   "
                << input.item->name
                << " "
                << input.rate
                << "/min\n";
        }


        std::cout << " Outputs:\n";

        for (auto& output : machine->outputs)
        {
            std::cout
                << "   "
                << output.item->name
                << " "
                << output.rate
                << "/min\n";
        }

        std::cout << "\n";
    }
}


void FactoryPrinter::PrintMachineGroups(
    const FactoryGraph& graph
)
{
    std::cout << "\nGrouped Machines:\n";


    auto groups = GroupMachines(graph);


    for (auto& group : groups)
    {
        std::cout
            << "  "
            << group.machines.size()
            << "x "
            << group.recipe->machine
            << " - "
            << group.recipe->name
            << "\n";
    }
}


void FactoryPrinter::PrintConnections(
    const FactoryGraph& graph
)
{
    std::cout << "\nConnections:\n";


    for (auto* connection : graph.connections)
    {
        std::cout
            << "  "
            << connection->from->recipe->name
            << " -> "
            << connection->to->recipe->name
            << " : "
            << connection->item->name
            << " "
            << connection->rate
            << "/min\n";
    }
}


std::vector<FactoryPrinter::MachineGroup>
FactoryPrinter::GroupMachines(
    const FactoryGraph& graph
)
{
    std::vector<MachineGroup> groups;


    for (auto* machine : graph.machines)
    {
        bool found = false;


        for (auto& group : groups)
        {
            if (group.recipe == machine->recipe &&
                group.clockSpeed == machine->clockSpeed)
            {
                group.machines.push_back(machine);
                found = true;
                break;
            }
        }


        if (!found)
        {
            MachineGroup group;

            group.recipe = machine->recipe;
            group.clockSpeed = machine->clockSpeed;
            group.machines.push_back(machine);

            groups.push_back(group);
        }
    }


    return groups;
}


std::vector<const ProductionMachine*>
FactoryPrinter::GetProducers(
    const FactoryGraph& graph,
    Item* item
)
{
    std::vector<const ProductionMachine*> result;


    for (auto* machine : graph.machines)
    {
        for (auto& output : machine->outputs)
        {
            if (output.item == item)
            {
                result.push_back(machine);
                break;
            }
        }
    }


    return result;
}


float FactoryPrinter::GetTotalOutput(
    const std::vector<const ProductionMachine*>& machines,
    Item* item
)
{
    float total = 0;


    for (auto* machine : machines)
    {
        for (auto& output : machine->outputs)
        {
            if (output.item == item)
            {
                total += output.rate;
            }
        }
    }


    return total;
}



void FactoryPrinter::PrintFactoryGraph(
    const FactoryGraph& graph,
    Item* target
)
{
    std::cout
        << "\nFactory Graph\n"
        << "=============\n\n";


    std::set<Item*> visited;


    PrintItemBranch(
        graph,
        target,
        GetTotalOutput(
            GetProducers(graph, target),
            target
        ),
        0,
        visited
    );
}



void FactoryPrinter::PrintItemBranch(
    const FactoryGraph& graph,
    Item* item,
    float rate,
    int depth,
    std::set<Item*>& visited
)
{
    std::string indent(depth * 4, ' ');


    std::cout
        << indent
        << item->name
        << " "
        << std::fixed
        << std::setprecision(1)
        << rate
        << "/min\n";


    auto producers = GetProducers(
        graph,
        item
    );


    if (producers.empty())
    {
        std::cout
            << indent
            << "  (resource)\n";

        return;
    }


    auto groups = GroupMachines(graph);


    for (auto& group : groups)
    {
        bool produces = false;


        for (auto* machine : group.machines)
        {
            for (auto& output : machine->outputs)
            {
                if (output.item == item)
                {
                    produces = true;
                }
            }
        }


        if (!produces)
            continue;


        std::cout
            << indent
            << "+-- "
            << group.machines.size()
            << "x "
            << group.recipe->machine
            << " - "
            << group.recipe->name
            << "\n";


        for (auto* machine : group.machines)
        {
            for (auto& input : machine->inputs)
            {
                float totalInput =
                    input.rate *
                    group.machines.size();


                PrintItemBranch(
                    graph,
                    input.item,
                    totalInput,
                    depth + 1,
                    visited
                );
            }


            break;
        }
    }
}



void FactoryPrinter::PrintProductionTree(
    const ProductionNode& node,
    int depth
)
{
    std::cout
        << std::string(depth * 2, ' ')
        << node.item->name
        << " x"
        << node.rate
        << "\n";


    for (auto& child : node.children)
    {
        PrintProductionTree(
            child,
            depth + 1
        );
    }
}



void FactoryPrinter::PrintMachineRequirements(
    const FactoryGraph& graph
)
{
    std::cout << "\nMachines Required:\n";


    for (auto& machine : graph.machineRequirements)
    {
        std::cout
            << machine.first->machine
            << " "
            << machine.first->name
            << " x"
            << machine.second
            << "\n";
    }
}



void FactoryPrinter::PrintResources(
    const FactoryGraph& graph
)
{
    std::cout << "\nResources:\n";


    for (auto& resource : graph.resources)
    {
        std::cout
            << resource.first->name
            << " "
            << resource.second
            << "/min\n";
    }
}



void FactoryPrinter::PrintBuildOrder(
    const FactoryGraph& graph
)
{
    std::cout
        << "\nBuild Order:\n"
        << "============\n";


    auto groups = GroupMachines(graph);


    int step = 1;


    for (auto& group : groups)
    {
        std::cout
            << step++
            << ". "
            << group.machines.size()
            << "x "
            << group.recipe->machine
            << " - "
            << group.recipe->name
            << "\n";
    }
}