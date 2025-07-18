#pragma once

#include <Components.hpp>
#include <systems/System.hpp>

namespace Systems
{
    /** \brief Updates child transform positions based on their parents */
    struct TransformParent : public System<Components::Transform>
    {
        // Must be defined, called every frame
        void update(float deltaTime, App &app) override;
    };

} // namespace Systems
