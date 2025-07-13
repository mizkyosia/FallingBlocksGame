#include <entities/Player.hpp>
#include <cmath>

namespace Entities
{
    Player::Player(sf::Texture &texture, sf::Vector2f position) : m_sprite(texture)
    {
        // Center our sprite
        auto spriteSize = m_sprite.getLocalBounds().size;
        m_sprite.setOrigin(sf::Vector2f{spriteSize.x / 2.f, spriteSize.y / 2.f});

        // And set its position
        m_sprite.setPosition(position);
    }

    void Player::update(sf::Time &deltaTime, FrameInputs newFrameInputs)
    {
        float deltaS = deltaTime.asSeconds();

        if (!m_onGround)
            m_velocity.y -= Gravity * deltaS;
        m_velocity.x = std::lerp(m_velocity.x, 0, (m_onGround ? GroundFriction : AirFriction) * deltaS);

        m_sprite.move(m_velocity);
    }
}
