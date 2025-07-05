#include <scenes/Game.hpp>
#include <thread>
#include <GlobalResources.hpp>
#include <iostream>

namespace Scenes
{
    Game::Game(SceneManager &sceneManager, sf::RenderWindow &renderWindow) : Scene(sceneManager, renderWindow, 1),
                                                                             m_text(GlobalResources::getInstance().uiFont)
    {
        std::cout << "beginning loading" << std::endl;

        m_text.setString("J'ai perdu au jeu");

        auto textSize = m_text.getLocalBounds().size;
        m_text.setOrigin(sf::Vector2f(textSize.x / 2.f, textSize.y / 2.f));

        auto windowSize = m_window.getSize();
        m_text.setPosition(sf::Vector2f(windowSize.x / 2.f, windowSize.y / 2.f));

        // Artifical loading time
        // std::this_thread::sleep_for(std::chrono::seconds(2));

        // Stress test for loading images
        for(int i =0; i < 100; i++) {
            if(i%2) m_texture.loadFromFile("assets/images/teto.png");
            else m_texture.loadFromFile("assets/images/cat.jpg");
        }

        // As we can see, they properly load in the background without blocking the main thread
    }

    void Game::draw()
    {
        m_window.draw(m_text);
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

            m_text.move(movement);
        }
    };
    void Game::update(const sf::Time &deltaTime) {

    };
}