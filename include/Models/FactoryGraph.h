#pragma once

#include <vector>
#include <unordered_map>

#include "Models/ProductionMachine.h"
#include "Models/MachineConnection.h"


struct FactoryGraph
{
    std::vector<ProductionMachine*> machines;
    std::vector<MachineConnection*> connections;

    std::unordered_map<Item*, float> resources;

    std::unordered_map<Recipe*, float> machineRequirements;
};