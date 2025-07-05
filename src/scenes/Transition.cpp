#include <scenes/Transition.hpp>
#include <SceneManager.hpp>

namespace Scenes
{
    Transition::Transition(SceneManager &sceneManager, sf::RenderWindow &renderWindow) : Scene(sceneManager, renderWindow, 1 << 0)
    {
        auto texSize = m_catTexture.getSize();
        m_catSprite.setOrigin(sf::Vector2f{texSize.x / 2.f, texSize.y / 2.f});

        auto windowSize = m_window.getSize();
        m_catSprite.setPosition(sf::Vector2f{windowSize.x / 2.f, windowSize.y / 2.f});
    };

    void Transition::draw()
    {
        m_window.draw(m_catSprite);
    };

    void Transition::handleEvent(const sf::Event &event) {

    };

        void Transition::update(const sf::Time &deltaTime)
    {
        if (!m_activated)
        {
            m_activated = true;
            m_manager.requestBuild<Scenes::Game>();
        }

        m_catSprite.rotate(sf::degrees(90.0f * deltaTime.asSeconds()));
    };
}