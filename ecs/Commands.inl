#pragma once
#include "Commands.hpp"
#include "World.hpp"
#include "Archetype.hpp"

inline Commands::Commands(World &world) : m_world(world) {};

template <typename... Components>
inline EntityCommands Commands::spawn(Components... components)
{
    EntityCommands cmd = m_world.spawn();
    (m_world.m_commandQueue.push_back(AnyCommand(EntityCommand{
        .entity = cmd.entity(),
        .action = AddComponentCommand{
            .component = m_world.getComponentID<Components>(),
            .data = std::static_pointer_cast<void>(std::make_shared<Components>(components))}})),
    ...);
    return cmd;
}

inline EntityCommands Commands::entity(Entity entity)
{
    return EntityCommands(&m_world, entity);
}


inline EntityCommands::EntityCommands(World *world, Entity entity) : m_world(world), m_entity(entity) {};

inline EntityCommands& EntityCommands::operator=(const EntityCommands& other) {
    m_world = other.m_world;
    m_entity = other.m_entity;
    return *this;
}

template <typename Component>
inline Component *EntityCommands::getComponent()
{
    return m_world->getArchetype(m_entity).get<Component>();
};

inline EntityCommands::EntityCommands(const EntityCommands &other) : m_entity(other.m_entity), m_world(other.m_world)
{
}

inline Entity EntityCommands::entity()
{
    return m_entity;
}

template <typename Component>
inline void EntityCommands::insertComponent(Component component)
{
    m_world->m_commandQueue.push_back(AnyCommand(AddComponentCommand{
        .entity = m_entity,
        .component = m_world->getComponentID<Component>(),
        .data = std::make_shared(component)}));
}

template <typename Component>
inline void EntityCommands::removeComponent()
{
    m_world->m_commandQueue.push_back(AnyCommand(AddComponentCommand{
        .entity = m_entity,
        .component = m_world->getComponentID<Component>()}));
}

inline void EntityCommands::destroy()
{
    m_world->m_commandQueue.push_back(AnyCommand(EntityCommand{.entity = m_entity, .action = DespawnEntityCommand{}}));
}