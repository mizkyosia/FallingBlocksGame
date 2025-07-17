#pragma once
#include <SFML/Graphics/Sprite.hpp>
#include <components/Component.hpp>
#include <assets/Texture.hpp>

namespace Components
{
    struct Sprite : public Component
    {
        DECLARE_COMPONENT()
        
    public:
        sf::Sprite sprite;
        Assets::Texture texture;

        Sprite(Assets::Texture texture) : texture(texture), sprite(*texture) {};
        Sprite() : texture("assets/images/teto.png"), sprite(*texture) {};
    };
} // namespace Components
