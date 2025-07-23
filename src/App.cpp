#include <iostream>

#include <Assets.hpp>
#include <Components.hpp>
#include <App.hpp>

#include <Test.hpp>

#include <Systems.hpp>

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

        AssetManager::PoolLoadingAssets();

        SystemManager::Update(dt, *this);

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

    // Register our systems
    registerSystems<Systems::TransformParent, Systems::SpriteDisplay>();

    Assets::Texture teto{"assets/images/teto.png"};

    std::cout << "Texture loading state : " << teto.loaded() << std::endl;

    auto windowSize = m_window.getSize();

    Entity ent;
    auto &transform = ent.addComponent(
        Components::Transform{
            .position = sf::Vector2f{windowSize.x / 2.f, windowSize.y / 2.f}});
    auto &sprite = ent.addComponent(Components::Sprite(teto));

    std::cout << "Transform position : " << transform.position.x << '|' << transform.position.y << std::endl;

    loop();

    m_window.close();
}