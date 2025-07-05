#pragma once
#include <Scene.hpp>
#include <gui/Button.hpp>

namespace Scenes
{
    class Intro : public Scene
    {
    private:
        sf::Texture m_tex{};

        Button m_button{};

    public:
        Intro(SceneManager &sceneManager, sf::RenderWindow &renderWindow);
        ~Intro() = default;

        void setup() override;
        void draw() override;
        void handleEvent(const sf::Event &event) override;
        void update(const sf::Time &deltaTime) override;
    };

} // namespace Scenes
