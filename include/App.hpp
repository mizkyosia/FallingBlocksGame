#pragma once
#include <SFML/Graphics.hpp>
#include <boost/core/demangle.hpp>

#include <ECS.hpp>
#include <assets/Asset.hpp>

using TextureAsset = Asset<sf::Texture>;

class App
{
private:

    World& world;

    void loop(Resource<sf::RenderWindow> window);

public:
    App();
    ~App();
    
    void run();
};
