#pragma once

#include "Models/ProductionMachine.h"
#include "Models/Item.h"


struct MachineConnection
{
    ProductionMachine* from = nullptr;
    ProductionMachine* to = nullptr;

    Item* item = nullptr;

    float rate = 0;
};