#pragma once
#include <bitset>
#include <cstdint>
#include <unordered_map>
#include <bits/shared_ptr.h>

#include <managers/ComponentManager.hpp>

#define DECLARE_COMPONENT()                                                                     \
private:                                                                                        \
    inline static ComponentManager::ComponentID s_ComponentID{ComponentManager::MaxComponents}; \
                                                                                                \
public:                                                                                         \
    friend ::App;                                                                         \
    inline static ComponentManager::ComponentID ComponentID() { return s_ComponentID; };

// Forward declaration for the macro
class App;