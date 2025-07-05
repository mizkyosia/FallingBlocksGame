#pragma once
#include <Scene.hpp>
#include <SFML/Audio.hpp>

namespace Scenes
{
    class Template : public Scene
    {
    private:
        sf::Image m_image;
        sf::Sound m_sound;

    public:
        Template(SceneManager &sceneManager, sf::RenderWindow &renderWindow);
        ~Template() = default;

        void setup() override;
        void draw() override;
        void handleEvent(const sf::Event &event) override;
        void update(const sf::Time &deltaTime) override;
    };

} // namespace Scenes
