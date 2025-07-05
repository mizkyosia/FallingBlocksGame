#include <App.hpp>
#include <scenes/Intro.hpp>
#include <GlobalResources.hpp>
#include <iostream>

App::App() : m_window(sf::VideoMode({1920u, 1080u}), "Test project", sf::State::Windowed),
             m_sceneManager(m_window)
{
}

App::~App()
{
}

void App::loop()
{
    bool open = true;
    GlobalResources &resources = GlobalResources::getInstance();

    resources.buttonShader.setUniform("resolution", sf::Glsl::Vec2(m_window.getSize()));

    while (open)
    {
        // Reset mouse click
        resources.mouseState.clicked = false;

        // Fetch events & apply them
        while (const std::optional<sf::Event> event = m_window.pollEvent())
        {
            // Close the game if it has been requested
            if (event->is<sf::Event::Closed>())
                open = false;

            // If the mouse is down, report it
            if (auto ev = event->getIf<sf::Event::MouseButtonPressed>())
                if (ev->button == sf::Mouse::Button::Left)
                {
                    resources.mouseState.pressedPosition = m_window.mapPixelToCoords(ev->position);
                    resources.mouseState.down = true;

                    std::cout << "Button down" << std::endl;
                }

            // If the mouse was released, report it
            if (auto ev = event->getIf<sf::Event::MouseButtonReleased>())
                if (ev->button == sf::Mouse::Button::Left)
                {
                    // If it was previously down, a click has been done this frame
                    if (resources.mouseState.down = true)
                        resources.mouseState.clicked = true;

                    resources.mouseState.down = false;
                }

            // If the mouse was moved, update its position
            if (auto ev = event->getIf<sf::Event::MouseMoved>())
                resources.mouseState.currentPosition = m_window.mapPixelToCoords(ev->position);

            m_sceneManager.handleEvent(*event);
        }

        sf::Time currentTime = m_clock.getElapsedTime();
        sf::Time deltaTime = currentTime - m_previousTime;

        // Update button shader
        resources.buttonShader.setUniform("time", currentTime.asSeconds());

        // Update scenes with delta time
        m_sceneManager.update(deltaTime);

        // Clear the window with a single color
        m_window.clear(sf::Color::Black);

        // Render the scenes
        m_sceneManager.draw();

        // Then display the result!
        m_window.display();

        m_previousTime = currentTime;
    }
}

void App::run()
{
    // Initialize the scene manager with the intro scene
    m_sceneManager.run<Scenes::Intro>();

    loop();

    m_window.close();
}