#include <iostream>

#include "Entity.inl"
#include <Assets.hpp>
#include <Components.hpp>
#include <App.hpp>

#include <systems/TransformParentSystem.hpp>

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
    registerAssetTypes<Assets::Texture,
                       Assets::Font,
                       Assets::Sound,
                       Assets::Shader>();

    // Register our component types
    // Yes we both need to declare them and register them afterwards
    // That is because the initialization order of the components matters,
    // and it happens in the order of the registration of the components (not their declaration)
    registerComponents<Components::Transform, Components::Sprite>();
    // Note that you can make multiple calls to any of the `registerXXX` functions
    registerComponents<Components::Collider>();

    registerSystems<Systems::TransformParentSystem>();

    Assets::Texture teto{"assets/images/teto.png"};

    std::cout << "Texture loading state : " << teto.loaded() << std::endl;

    Entity ent;
    auto &sprite = ent.addComponent<Components::Sprite>(Components::Sprite(teto));
    auto &transform = ent.addComponent<Components::Transform>(Components::Transform());

    std::cout << "Transform position : " << transform.position.x << '|' << transform.position.y << std::endl;

    loop();

    m_window.close();
}