#pragma once
#include <scenes/Scene.hpp>
#include <SFML/Audio.hpp>

namespace Scenes
{
    class Game : public Scene
    {
    private:
        sf::Texture m_texture{"assets/images/cat.jpg"};

        sf::Sprite m_player{m_texture};

    public:
        Game(SceneManager &sceneManager, sf::RenderWindow &renderWindow);
        ~Game() = default;

        void draw() override;
        void handleEvent(const sf::Event &event) override;
        void update(const sf::Time &deltaTime) override;
    };

} // namespace Scenes
