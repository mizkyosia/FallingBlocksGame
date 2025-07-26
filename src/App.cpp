#include <iostream>

#include <Components.hpp>
#include <App.hpp>

#include <ECS.hpp>

App::App() : m_window(sf::VideoMode({1920u, 1080u}), "Test project", sf::Style::None, sf::State::Windowed)
{
    m_window.setFramerateLimit(144);
}

App::~App()
{
}

void App::loop()
{
    bool open = true;

    auto frameTimePoint = std::chrono::high_resolution_clock::now();
    float dt = 0;

    while (open)
    {
        using namespace std::chrono;

        while (const std::optional<sf::Event> event = m_window.pollEvent())
        {
            using sf::Event;

            // Close the game if it has been requested
            if (event->is<Event::Closed>())
                open = false;
        }

        // Calculate delta time
        auto startFrame = high_resolution_clock::now();
        duration<float> dur = startFrame - frameTimePoint;
        dt = dur.count();
        frameTimePoint = startFrame;

        m_window.display();
    }
}

void App::run()
{
    World &world = World::Create<Components::Transform>();

    auto cmds = world.commands();

    auto entityCmds = cmds.spawn(Components::Transform{.position = {50.f, 50.f}});

    // Register our systems

    auto windowSize = m_window.getSize();

    loop();

    m_window.close();
}