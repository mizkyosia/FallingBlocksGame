#pragma once
#include <SFML/Graphics.hpp>

namespace Entities
{
    class Player
    {
    public:
        struct FrameInputs
        {
            bool up = false,
                 down = false,
                 left = false,
                 right = false,
                 jump = false,
                 interact = false;
        };

    private:
        FrameInputs m_oldFrameInputs;

        sf::Sprite m_sprite;

        sf::Vector2f m_velocity;

        bool m_onGround;

        float m_timeCoyote, m_timeBuffer;

    public:
        static constexpr float Gravity = 9.81f,
                           AirFriction = .5f,
                           GroundFriction = 5.f;

        Player(sf::Texture &texture, sf::Vector2f position);
        ~Player() = default;

        void update(sf::Time &deltaTime, FrameInputs newFrameInputs);
    };
} // namespace Entities
