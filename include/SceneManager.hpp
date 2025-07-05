#pragma once
#include <vector>
#include <SFML/Graphics.hpp>
#include <future>
#include <Scene.hpp>

class SceneManager
{
private:
    /**
     * Vector containing all scenes currently defined in the game
     * We do not use a stack for iteration purposes
     */
    std::vector<std::unique_ptr<Scene>> m_scenes;

    /**
     * Mutex protecting access to the scenes vector
     */
    std::mutex m_scenesMutex;

    /**
     * Keep a reference to the render window
     */
    sf::RenderWindow &m_window;

    /**
     * Should the scene manager load the next scene ?
     */
    bool m_next;

    /**
     * Should the scene manager go back to the previous scene ?
     */
    bool m_previous;

    /**
     * Checks if the scene needs to be changed, and does it if necessary
     */
    void changeScene();

public:
    SceneManager(sf::RenderWindow &window);
    ~SceneManager() = default;

    /**
     * Inializes a scene of the given type and instantly push it onto the stack
     * Used to initialize the first scene of the application
     */
    template <typename TScene>
        requires(std::derived_from<TScene, Scene>)
    void run()
    {
        auto startScene = std::make_unique<TScene>(*this, m_window);

        m_scenes.push_back(std::move(startScene));
    };

    /**
     * Passes the event to the active scene
     */
    void handleEvent(const sf::Event &event);

    /**
     * Updates the currently active scenes
     */
    void update(sf::Time &deltaTime);

    /**
     * Draws the scenes
     */
    void draw();

    /**
     * Orders the scene manager to go back to the previous scene
     */
    void previousScene();

    /**
     * Initializes a new scene from given class, then pushes it onto the stack and pauses the previous scenes
     */
    template <typename TScene>
        requires(std::derived_from<TScene, Scene>)
    void requestBuild(Scene *parentScene)
    {
        std::async(std::launch::async,
                   [&]
                   {
                       // Create our new scene (this is where all the heavy work will be done)
                       std::unique_ptr<TScene> newScenePtr = std::make_unique<TScene>(*this, m_window);

                        // Scoping shenanigans for locking
                       {
                           // Now that our scene is built, lock the necessary resources to add it into our game
                           std::lock_guard<std::mutex> guard(m_scenesMutex);

                           // Fetch its scene layers
                           auto nextSceneLayers = newScenePtr->sceneLayers();

                           // Erase any scenes on the same layer(s), pause all others
                           m_scenes.erase(std::remove_if(m_scenes.begin(), m_scenes.end(), [&](std::unique_ptr<Scene> &p)
                                                         { p->pause();
                                                            return p->sceneLayers() & nextSceneLayers; }));

                           // Resume the new scene & add it onto the stack
                           newScenePtr->resume();
                           m_scenes.push_back(std::move(newScenePtr));
                       }
                       // The lock guard is now destroyed, scenes can be accessed freely by any thread now
                   });
    }
};
