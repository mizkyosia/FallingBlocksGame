#pragma once
#include <SFML/Graphics.hpp>

namespace Entities
{
    class Block
    {
    protected:
        sf::Sprite m_sprite;
        sf::Shader *m_shader;

        sf::Vector2f m_velocity;

    public:
        static const float Gravity = 5.f;
        static const float MaxSpeed = 10.f;

        Block(sf::Texture &texture, sf::Shader *shader) : m_sprite(texture), m_shader(shader) {};
        ~Block() = default;

        virtual bool playerCollision(const sf::FloatRect &hitbox) = 0;
        virtual bool playerKill(const sf::FloatRect &hitbox) = 0;

        virtual void update(float deltaTime, std::vector<Block> &blocks)
        {
            m_velocity.y = std::min(m_velocity.y + Gravity * deltaTime, MaxSpeed);

            sf::Vector2f newPos = m_sprite.getPosition();
        };

        virtual void draw(sf::RenderWindow &window) const
        {
            window.draw(m_sprite, m_shader);
        };
    };
} // namespace Entities
