#pragma once
#include <Scene.hpp>
#include <SFML/Audio.hpp>

namespace Scenes
{
    class Game : public Scene
    {
    private:
        sf::Text m_text;

        sf::Texture m_texture{"assets/images/cat.jpg"};

    public:
        Game(SceneManager &sceneManager, sf::RenderWindow &renderWindow);
        ~Game() = default;

        void draw() override;
        void handleEvent(const sf::Event &event) override;
        void update(const sf::Time &deltaTime) override;
    };

} // namespace Scenes
