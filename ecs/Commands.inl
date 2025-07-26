#pragma once
#include "Commands.hpp"
#include "World.hpp"
#include "Archetype.hpp"

inline Commands::Commands(World &world) : m_world(world) {};

template <typename... Components>
inline EntityCommands Commands::spawn(Components... components)
{
    Entity e = m_world.spawn();
    (m_world.m_commandQueue.push_back(AnyCommand(AddComponentCommand{.component = m_world.getComponentID<Components>(), .entity = e, .data = std::make_shared(components)})), ...);
    return EntityCommands(m_world, e);
}

inline EntityCommands Commands::entity(Entity entity)
{
    return EntityCommands(m_world, entity);
}

inline EntityCommands::EntityCommands(World &world, Entity entity) : m_world(world), m_entity(entity) {};

template <typename Component>
inline Component *EntityCommands::getComponent()
{
    return m_world.m_entityToArchetype[m_entity]->get<Component>();
};


template <typename Component>
inline void EntityCommands::insertComponent(Component component)
{
    m_world.m_commandQueue.push_back(AnyCommand(AddComponentCommand{
        .entity = m_entity,
        .component = m_world.getComponentID<Component>(),
        .data = std::make_shared(component)}));
}

template <typename Component>
inline void EntityCommands::removeComponent()
{
    m_world.m_commandQueue.push_back(AnyCommand(AddComponentCommand{
        .entity = m_entity,
        .component = m_world.getComponentID<Component>()}));
}

inline void EntityCommands::destroy()
{
    m_world.m_commandQueue.push_back(AnyCommand(EntityCommand{.entity = m_entity, .action = DespawnEntityCommand{}}));
}