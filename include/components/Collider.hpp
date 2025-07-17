#pragma once
#include <SFML/Graphics/Rect.hpp>

#include <components/Component.hpp>

namespace Components
{
    struct Collider : public Component
    {
        DECLARE_COMPONENT()

        sf::FloatRect rect;

        Collider() : rect() {};
    };
} // namespace Components
