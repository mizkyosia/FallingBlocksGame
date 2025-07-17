#pragma once
#include <bitset>
#include <cstdint>
#include <unordered_map>
#include <bits/shared_ptr.h>

#include <managers/ComponentManager.hpp>

// Forward declaration for the macro
class App;

#define DECLARE_COMPONENT()                                                                     \
private:                                                                                        \
    inline static ComponentManager::ComponentID s_ComponentID{ComponentManager::MaxComponents}; \
                                                                                                \
public:                                                                                         \
    friend class ::App;                                                                         \
    inline static ComponentManager::ComponentID ComponentID() { return s_ComponentID; };

namespace Components
{
    /**
     * \brief Virtual class, base of all components
     *
     * Used for abstraction & component management purposes
     */
    struct Component
    {
        Component() = default;
        virtual ~Component() = default;
    };

} // namespace Components