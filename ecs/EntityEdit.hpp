#pragma once
#include "Global.hpp"
#include <unordered_map>
#include <bits/shared_ptr.h>

struct EntityEdit
{
    Signature signature;
    std::unordered_map<ComponentID, std::shared_ptr<void>> componentsToInsert;
    bool justSpawned = false;
    bool despawning = false;
};
