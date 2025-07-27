#include <iostream>

#include <Components.hpp>
#include <App.hpp>

#include <assets/AssetManager.hpp>

#include <ECS.hpp>

App::App() : world(World::Create<
                   Components::Transform,
                   Components::Collider,
                   Components::Sprite>())
{
}

App::~App()
{
}

void App::loop(Resource<sf::RenderWindow> window)
{
    bool open = true;

    auto frameTimePoint = std::chrono::high_resolution_clock::now();
    float dt = 0;

    while (open)
    {
        while (const std::optional<sf::Event> event = window->pollEvent())
        {
            using sf::Event;

            // Close the game if it has been requested
            if (event->is<Event::Closed>())
                open = false;
        }

        world.tick();

        window->display();
    }
}

// First system test
void test(
    // Fetching access to the world
    World &world,
    // More control over the world
    Commands cmd,
    // Query selecting all entities
    Query<None, Entity> &query)
{
    std::cout << "Yippeeeeee" << std::endl;
    // Spawn entities until there are 200 of them
    if (query.size() < 200)
        cmd.spawn();
};

// Second system test
void test2(
    // First query, with a filter
    Query<With<Components::Transform>, Entity, EntityCommands> &query,
    // Second query, no filter but fetching a component
    Query<None, Components::Transform> &otherQuery,
    // Fetching a resource
    Resource<sf::RenderWindow> window)
{
    std::cout << "Counted " << query.size() << " & " << otherQuery.size() << " living entities matching queries 1 & 2 :3 | Window width : " << window->getSize().x << std::endl;
    // Iterating queries
    for (auto [e, cmd] : query)
    {
        std::cout << "Entity n°" << e << std::endl;
    }
};

void App::run()
{

    Commands cmds = world.commands();

    // Spawn a entity with an integrated `Transform`
    EntityCommands entityCmds = cmds.spawn(Components::Transform{.position = {50.f, 50.f}});

    // Insert new resources and get handles to them
    auto [assets, window] = world.insertResources(AssetManager(), sf::RenderWindow(sf::VideoMode({1920u, 1080u}), "Test project", sf::Style::None, sf::State::Windowed));

    window->setFramerateLimit(144);

    // Add our systems
    world.addSystems(test, test2);

    auto windowSize = window->getSize();

    loop(window);

    window->close();
}