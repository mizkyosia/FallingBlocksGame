#pragma once
#include <SFML/Graphics.hpp>
#include <boost/core/demangle.hpp>

#include <ECS.hpp>

using TextureAsset = Asset<sf::Texture>;

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
    
    sf::RenderWindow &window() { return m_window; };

    void run();
};
