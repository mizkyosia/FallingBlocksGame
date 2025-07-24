#pragma once
#include <SFML/Graphics/Rect.hpp>


namespace Components
{
    struct Collider
    {

        sf::FloatRect rect;

        Collider() : rect() {};
    };
} // namespace Components
