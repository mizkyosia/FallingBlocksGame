#include <managers/SceneManager.hpp>
#include <scenes/Scene.hpp>
#include <algorithm>

SceneManager::SceneManager(sf::RenderWindow &window) : m_window(window), m_next(false), m_previous(false)
{
}

void SceneManager::changeScene()
{
    // Do we need to go back ?
    if (m_previous)
    {
        // Remove active state
        if (!m_scenes.empty())
            m_scenes.pop_back();

        // Resume previously active scene
        if (!m_scenes.empty())
            m_scenes.back()->resume();

        m_previous = false;
    }
}

void SceneManager::handleEvent(const sf::Event &event)
{
    if (m_scenes.empty())
        return;
    m_scenes.back()->handleEvent(event);
}

void SceneManager::update(sf::Time &deltaTime)
{
    // Change the scenes if needed
    changeScene();

    if (m_scenes.empty())
        return;

    // Update only the first state
    m_scenes.back()->update(deltaTime);
}

void SceneManager::setActiveLayers(unsigned int layers, bool deleteActive)
{
    // Erase any scenes on the same layer(s), pause all others
    m_scenes.erase(std::remove_if(m_scenes.begin(), m_scenes.end(), [&](std::unique_ptr<Scenes::Scene> &p)
                                  { p->pause();
                                                            return deleteActive && !p->paused(); }));
}

void SceneManager::draw()
{
    for (auto &scene : m_scenes)
        scene->draw();
}