#include <vector>
#include <SFML/Graphics.hpp>

class Scene;

class SceneManager
{
private:
    /**
     * Vector containing all scenes currently defined in the game
     * We do not use a stack for iteration purposes
     */
    std::vector<std::unique_ptr<Scene>> m_scenes;

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
        auto startScene = init<TScene>();

        m_scenes.push_back(std::move(startScene));
    };

    /**
     * Passes the event to the active scene
     */
    void handleEvent(const sf::Event &event);

    /**
     * Updates the currently active scenes
     */
    void update(sf::Time& deltaTime);

    /**
     * Draws the scenes
     */
    void draw();

    /**
     * Orders the scene manager to go back to the previous scene
     */
    void previousScene();

    /**
     * Initializes a new scene from given class, then returns it
     */
    template <typename TScene>
        requires(std::derived_from<TScene, Scene>)
    std::unique_ptr<TScene> init()
    {
        auto newScenePtr = std::make_unique<TScene>(*this, m_window);

        return newScenePtr;
    }
};
