#include <scenes/Game.hpp>
#include <thread>
#include <GlobalResources.hpp>
#include <iostream>

namespace Scenes
{
    Game::Game(SceneManager &sceneManager, sf::RenderWindow &renderWindow) : Scene(sceneManager, renderWindow, 1)
    {
    }

    void Game::draw()
    {
        m_window.draw(m_player);
    };

    void Game::handleEvent(const sf::Event &event)
    {
        if (auto ev = event.getIf<sf::Event::KeyPressed>())
        {
            sf::Vector2f movement{0, 0};

            switch (ev->code)
            {
            case sf::Keyboard::Key::Up:
                movement.y++;
                break;

            case sf::Keyboard::Key::Down:
                movement.y--;
                break;
            case sf::Keyboard::Key::Right:
                movement.x++;
                break;
            case sf::Keyboard::Key::Left:
                movement.x--;
                break;

            default:
                break;
            }
        }
    };

    void Game::update(const sf::Time &deltaTime) {
        
    };
}