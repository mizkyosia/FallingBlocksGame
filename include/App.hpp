#pragma once
#include <SFML/Graphics.hpp>
#include <managers/SceneManager.hpp>
#include <managers/AssetsManager.hpp>

class App
{
private:
    /**
     * The SFML Window & Renderer
     */
    sf::RenderWindow m_window;

    /**
     * SFML Clock to keep track of delta time
     */
    sf::Clock m_clock;

    /**
     * Time at the previous frame
     */
    sf::Time m_previousTime;

    /**
     * Scene manager
     */
    SceneManager m_sceneManager;

    AssetsManager m_assetsManager;

    void loop();

public:
    App();
    ~App();

    void run();
};
