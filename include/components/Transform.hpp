#pragma once
#include <SFML/System/Vector2.hpp>

#include <components/Component.hpp>

namespace Components
{
    struct Transform : public Component
    {
        DECLARE_COMPONENT()

        sf::Vector2f position;
        float angle;
        sf::Vector2f scale;

        sf::Vector2f localPosition;
        float localAngle;
        sf::Vector2f localScale;

        Transform() : position{0, 0}, angle(0), scale{0, 0}, localPosition(position), localAngle(angle), localScale(scale) {};
    };
} // namespace Components
