#pragma once
#include <SFML/Graphics/Sprite.hpp>
#include <assets/Texture.hpp>

namespace Components
{
    struct Sprite
    {
        
    public:
        sf::Sprite sprite;
        Assets::Texture texture;

        Sprite(Assets::Texture texture) : texture(texture), sprite(*texture) {};
        Sprite() : texture("assets/images/teto.png"), sprite(*texture) {};
    };
} // namespace Components
