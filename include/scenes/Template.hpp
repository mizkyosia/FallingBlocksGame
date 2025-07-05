#pragma once
#include <Scene.hpp>
#include <SFML/Audio.hpp>
#include <SceneManager.hpp>

namespace Scenes
{
    /**
     * Template scene, for inspiration & good habits to take about declaring scenes
     * Do not instantiate or otherwise use in the application
     */
    class Template : public Scene
    {
    private:
    // Assets loaded in the declaration or the constructor will be loaded on a separate thread before the scene is displayed,
    // to not block the application from running
    // As such, it is best to load assets in this manner, rather than loading them after the scene is built
        sf::Image m_image{"assets/images/cat.jpg"};
        sf::Music m_music{"assets/sounds/meow.mp3"};

    public:
        Template(SceneManager &sceneManager, sf::RenderWindow &renderWindow);
        ~Template() = default;

        void draw() override;
        void handleEvent(const sf::Event &event) override;
        void update(const sf::Time &deltaTime) override;
    };

} // namespace Scenes
