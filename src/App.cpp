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
    // Control over the world (spawning enities etc)
    Commands cmd,
    // Query selecting all entities without a transform (should be <= 199)
    // Fetches [Entity] type
    Query<Without<Components::Transform>, Entity> &query,
    // All entities
    Query<None, Entity> &otherQuery)
{
    std::cout << "Living entity count without Transform : " << query.size() << std::endl;
    // Spawn entities until there are 200 of them
    if (otherQuery.size() < 200)
        cmd.spawn();
};

// Second system test
void test2(
    // First query, with a filter (only one filter per query !!!)
    // Fetches [Entity, EntityCommands] types.
    // Entity is just an ID
    // EntityCommands allows for modification of an Entity, such as adding/removing components, or even despawning
    Query<With<Components::Transform>, Entity, EntityCommands> &query,
    // Second query, no filter but fetching a component
    // Fetched types are [Entity, Components::Transform*]
    // In queries, all components are transformed into pointers to the actual component for remote access
    Query<None, Entity, Components::Transform> &otherQuery,
    // Fetching a resource
    // Trivially copyable/movable, juste a pointer wrapper
    Resource<sf::RenderWindow> window)
{
    std::cout << "Counted " << query.size() << " & " << otherQuery.size() << " living entities matching queries 1 & 2 :3 | Window width : " << window->getSize().x << std::endl;
    // Iterating queries
    for (auto &[e, transform] : otherQuery)
    {
        std::cout << "Entity n°" << e << " at position " << transform->position.x << " | " << transform->position.y << std::endl;
        transform->position.x += 0.1f;
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