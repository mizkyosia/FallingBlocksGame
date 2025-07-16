#pragma once
#include <SFML/Graphics.hpp>

class App
{
private:
    /**
     * The SFML Window & Renderer
     */
    sf::RenderWindow m_window;

    void loop();

public:
    App();
    ~App();

    void run();
};
