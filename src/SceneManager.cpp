#include "SceneManager.hpp"
#include "Scene.hpp"
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

    // Fetch the next scene's ready state
    if (!m_scenes.empty() && m_scenes.back()->nextReady())
    {
        // Fetch the actual next scene
        auto newScene = m_scenes.back()->next();

        

        // Then pause the scene currently on top, if any remain
        if (!m_scenes.empty())
            m_scenes.back()->pause();

        // And resume & add the new one
        newScene->resume();
        m_scenes.push_back(std::move(newScene));
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

void SceneManager::draw()
{
    for (auto &scene : m_scenes)
        scene->draw();
}

void SceneManager::previousScene()
{
    m_previous = true;
}