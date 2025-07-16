#pragma once
#include <bitset>
#include <cstdint>
#include <unordered_map>
#include <bits/shared_ptr.h>

#include "managers/ComponentManager.hpp"

namespace Components
{
    /**
     * \brief Virtual class, base of all components
     *
     * Used for abstraction & component management purposes
     */
    template <typename T>
    struct Component
    {
        /** \brief Unique ID specific to a certain type of component. All classes directly derived from `Component` possess a different static ID */
        inline static volatile const ComponentManager::ComponentID ComponentID{ComponentManager::Register<T>()};
        Component() = default;
        virtual ~Component() = default;
    };

} // namespace Components