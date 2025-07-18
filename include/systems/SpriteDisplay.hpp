#pragma once

#include <SFML/System/Angle.hpp>
#include <Components.hpp>
#include <systems/System.hpp>

namespace Systems
{
    /** \brief Display sprites to the window */
    struct SpriteDisplay : public System<Components::Transform, Components::Sprite>
    {
        // Must be defined, called every frame
        void update(float deltaTime, App &app) override;
    };

} // namespace Systems
