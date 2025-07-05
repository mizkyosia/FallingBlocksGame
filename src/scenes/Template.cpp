#include <scenes/Template.hpp>
#include <SceneManager.hpp>

namespace Scenes
{
    Template::Template(SceneManager &sceneManager, sf::RenderWindow &renderWindow) : Scene(sceneManager, renderWindow, 1 << 0){};

    void Template::draw() {
        
    };
    void Template::handleEvent(const sf::Event &event) {
        
    };
    void Template::update(const sf::Time &deltaTime) {
        
    };
}