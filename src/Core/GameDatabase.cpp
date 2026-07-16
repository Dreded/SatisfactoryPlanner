#include "Core/GameDatabase.h"

#include <iostream>

bool GameDatabase::Load(const std::string& filename)
{
    std::cout << "Loading database: "
        << filename
        << std::endl;

    return true;
}