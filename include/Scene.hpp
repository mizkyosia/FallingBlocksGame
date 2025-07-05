#pragma once
#include <vector>
#include <SFML/Graphics.hpp>

class SceneManager;

/**
 * Represents a base game state (aka a "scene" in most game editors)
 * This scene has ownership of all assets used in its drawing, and it is its responsibility to load/unload them from memory
 */
class Scene
{
protected:
    /**
     * Keep a reference to the parent manager
     */
    SceneManager &m_manager;

    /**
     * Keep a reference to the window for drawing
     */
    sf::RenderWindow &m_window;

    /**
     * Is the scene ready for display ?
     */
    bool m_ready;
    /**
     * Is the scene currently displayed ?
     */
    bool m_paused;

    /**
     * The "replace layer" mask of the scene
     * Only scenes on the same layer bits replace each other, other layers are simply paused
     */
    unsigned short m_sceneLayers;

    /**
     * The next scene to be displayed, spawned by this one
     */
    std::unique_ptr<Scene> m_next;

public:
    /**
     * Creates a new scene
     * Do not forget to put "loaded = true;" at the end of the derived class' constructor to validate the scene's display
     */
    Scene(SceneManager &manager, sf::RenderWindow &window, unsigned short sceneLayers = 1) : m_manager(manager), m_window(window), m_ready(false), m_paused(true), m_sceneLayers(sceneLayers) {};
    virtual ~Scene() {};

    /**
     * Handles a given event. Called for each event, before updating
     */
    virtual void handleEvent(const sf::Event &event) = 0;
    /**
     * Updates the scene, e.g. the player's position & velocity, perform collisions...
     * Called each frame, before drawing & after updating events
     */
    virtual void update(const sf::Time &deltaTime) = 0;
    /**
     * Draws the scene to the screen.
     * Called each frame, after updating
     */
    virtual void draw() = 0;

    // ================== GETTERS ==================

    bool ready() const { return m_ready; }
    bool paused() const { return m_paused; }
    unsigned short sceneLayers() const { return m_sceneLayers; }
    /**
     * Gets the unique pointer to the next scene
     * \warning This moves the unique pointer !
     */
    [[nodiscard]] std::unique_ptr<Scene> next() { return std::move(m_next); }
    /**
     * Is the next scene ready to be displayed ?
     */
    bool nextReady() const { return m_next != nullptr && m_next->ready(); }

    // ================== SETTERS ==================

    /**
     * Pause the execution of the scene
     */
    virtual void pause() { m_paused = true; }
    /**
     * Resume the execution of the scene
     */
    virtual void resume() { m_paused = false; }
};