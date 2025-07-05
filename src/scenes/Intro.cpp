#include <scenes/Intro.hpp>
#include <scenes/Game.hpp>
#include <SceneManager.hpp>
#include <GlobalResources.hpp>
#include <iostream>

namespace Scenes
{
    Intro::Intro(SceneManager &sceneManager, sf::RenderWindow &renderWindow) : Scene(sceneManager, renderWindow, 1) {}

    void Intro::setup()
    {
        GlobalResources &res = GlobalResources::getInstance();

        // Load texture
        m_tex.loadFromFile("assets/images/teto.png");

        auto size = m_window.getSize();
        auto position = sf::Vector2f(size.x / 2, size.y / 2);

        auto spriteSize = m_tex.getSize();
        auto origin = sf::Vector2f(spriteSize.x / 2, spriteSize.y / 2);

        // Setup button
        {
            auto &rect = m_button.shape();

            rect.setSize(sf::Vector2f(spriteSize.x, spriteSize.y));
            rect.setPosition(position);
            rect.setOrigin(origin + sf::Vector2f{100.0, 100.0});
            rect.setFillColor(sf::Color::Blue);

            rect.setTexture(&m_tex);

            // Apply shader to button
            m_button.setShader(&res.buttonShader);
        }
    }

    void Intro::draw()
    {
        m_window.draw(m_button);
    }

    void Intro::handleEvent(const sf::Event &event)
    {
        if (auto key = event.getIf<sf::Event::KeyPressed>())
        {
            if (key->code == sf::Keyboard::Key::A)
            {
                std::cout << "Key A pressed\n";
            }
        }
    }

    void Intro::update(const sf::Time &deltaTime)
    {
        // if (m_button.clicked())
        //     m_next = m_manager.init<Scenes::Game>();

        m_button.shape().rotate(sf::degrees(deltaTime.asSeconds() * 90.0f));
    }
}