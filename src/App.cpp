#include <iostream>

#include <Entity.hpp>
#include <Assets.hpp>
#include <Components.hpp>
#include <App.hpp>

App::App() : m_window(sf::VideoMode({1920u, 1080u}), "Test project", sf::State::Windowed)
{
}

App::~App()
{
}

void App::loop()
{
    bool open = true;
    while (open)
    {
        while (const std::optional<sf::Event> event = m_window.pollEvent())
        {
            using sf::Event;

            // Close the game if it has been requested
            if (event->is<Event::Closed>())
                open = false;
        }
        /* code */

        m_window.display();
    }
}

void App::run()
{
    // Initialize the entity IDs we want
    Entity::InitIDs();

    // See components below for more information
    registerAssetType<Assets::Texture>();
    registerAssetType<Assets::Font>();
    registerAssetType<Assets::Sound>();
    registerAssetType<Assets::Shader>();

    // Register our component types
    // Yes we both need to declare them and register them afterwards
    // That is because the initialization order of the components matter
    registerComponent<Components::Transform>();
    registerComponent<Components::Sprite>();

    Assets::Texture teto{"assets/images/teto.png"};

    std::cout << "Texture loading state : " << teto.loaded() << std::endl;

    Entity ent;
    auto &sprite = ent.addComponent<Components::Sprite>(Components::Sprite(teto));
    auto &transform = ent.addComponent<Components::Transform>(Components::Transform());

    std::cout << "Transform position : " << transform.position.x << '|' << transform.position.y << std::endl;

    loop();

    m_window.close();
}