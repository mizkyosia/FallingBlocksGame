#pragma once
#include <SFML/Graphics/Sprite.hpp>
#include <ECS.hpp>

namespace Components
{
    struct Sprite
    {
        
    public:
        sf::Sprite sprite;
        Asset<sf::Texture> texture;

        Sprite(Asset<sf::Texture> texture) : texture(texture), sprite(*texture) {};
        Sprite() : texture(), sprite(*texture) {};
    };
} // namespace Components
