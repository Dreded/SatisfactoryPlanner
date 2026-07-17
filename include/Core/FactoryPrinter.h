#pragma once

#pragma once

#include <vector>
#include <set>

#include "Models/FactoryGraph.h"
#include "Models/ProductionNode.h"

class FactoryPrinter
{
public:

    static void PrintMachines(
        const FactoryGraph& graph
    );

    static void PrintMachineDetails(
        const FactoryGraph& graph
    );

    static void PrintMachineGroups(
        const FactoryGraph& graph
    );

    static void PrintConnections(
        const FactoryGraph& graph
    );

    static void PrintProductionTree(
        const ProductionNode& node,
        int depth = 0
    );

    static void PrintMachineRequirements(
        const FactoryGraph& graph
    );

    static void PrintResources(
        const FactoryGraph& graph
    );

    static void PrintBuildOrder(
        const FactoryGraph& graph
    );

    static void PrintFactoryGraph(
        const FactoryGraph& graph,
        Item* target
    );


private:

    struct MachineGroup
    {
        Recipe* recipe = nullptr;
        float clockSpeed = 0;
        std::vector<const ProductionMachine*> machines;
    };


    static std::vector<MachineGroup> GroupMachines(
        const FactoryGraph& graph
    );


    static std::vector<const ProductionMachine*> GetProducers(
        const FactoryGraph& graph,
        Item* item
    );


    static float GetTotalOutput(
        const std::vector<const ProductionMachine*>& machines,
        Item* item
    );


    static void PrintItemBranch(
        const FactoryGraph& graph,
        Item* item,
        float rate,
        int depth,
        std::set<Item*>& visited
    );
};