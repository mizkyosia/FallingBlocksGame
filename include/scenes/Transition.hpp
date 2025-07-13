#pragma once
#include <scenes/Scene.hpp>
#include <SFML/Audio.hpp>
#include <managers/SceneManager.hpp>
#include <scenes/Game.hpp>

namespace Scenes
{
    class Transition : public Scene
    {
    private:
        sf::Texture m_catTexture{"assets/images/cat.jpg"};
        sf::Sprite m_catSprite{m_catTexture};

        bool m_activated;

    public:
        Transition(SceneManager &sceneManager, sf::RenderWindow &renderWindow);
        ~Transition() = default;

        void draw() override;
        void handleEvent(const sf::Event &event) override;
        void update(const sf::Time &deltaTime) override;
    };

} // namespace Scenes
