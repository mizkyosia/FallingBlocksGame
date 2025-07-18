#pragma once
#include <SFML/System/Vector2.hpp>

#include <components/Component.hpp>

namespace Components
{
    struct Transform
    {
        DECLARE_COMPONENT()
    public:
        sf::Vector2f position{0, 0};
        float angle{0};
        sf::Vector2f scale{1, 1};

        sf::Vector2f localPosition{0, 0};
        float localAngle{0};
        sf::Vector2f localScale{1, 1};
    };
} // namespace Components
