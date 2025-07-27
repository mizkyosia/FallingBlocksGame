#include "ECS.hpp"
#include <variant>
#include "World.hpp"

World::World()
{
    // Fill entity IDs
    for (Entity i = 0; i < MAX_ENTITIES; i++)
        m_availableEntities.push(i);
};

Entity World::reserveEntity()
{
    Entity e = m_availableEntities.front();
    m_availableEntities.pop();
    return e;
}

Commands World::commands()
{
    return Commands(*this);
}

EntityCommands World::spawn()
{
    Entity e = reserveEntity();
    m_commandQueue.push_back(AnyCommand(EntityCommand{.entity = e, .action = SpawnEntityCommand{}}));
    return EntityCommands(this, e);
}

Archetype &World::getArchetype(const Signature &sig)
{
    auto it = m_archetypes.find(sig);
    if (it != m_archetypes.end())
        return it->second;

    auto [a, b] = m_archetypes.insert({sig, Archetype(*this, sig)});

    return a->second;
}

inline Archetype &World::getArchetype(Entity entity)
{
    return getArchetype(m_entityLocations[entity].signature);
}

void World::tick()
{
    // Check if we aren't ticking already
    if (m_ticking)
        return;
    m_ticking = true;

    // Run all systems
    for (auto &system : m_systems)
        system->run();

    // Accumulate queued operations for batch edits of entities

    std::unordered_map<Entity, EntityEdit> entityEdits;

    for (auto &command : m_commandQueue)
    {
        if (auto entityCommand = std::get_if<EntityCommand>(&command))
        {
            // Get or create the `EntityEdit`
            auto [edit, firstEdit] = entityEdits.try_emplace(entityCommand->entity);

            // If the entity is despawning this tick, ignore all updates & mark as despawning
            if (edit->second.despawning || std::holds_alternative<DespawnEntityCommand>(entityCommand->action))
            {
                edit->second.despawning = true;
                continue;
            }

            // Is the `Entity` spawning/despawning this tick ?
            bool spawningEntity = std::holds_alternative<SpawnEntityCommand>(entityCommand->action);

            // Sets the previous signature of the `Entity` if it exists, or `0` if  newly created entity
            if (spawningEntity)
            {
                edit->second.signature = 0;
                edit->second.justSpawned = true;
            }
            else if (firstEdit)
            {
                edit->second.signature = m_entityLocations[entityCommand->entity].signature;
            }

            // Add component to the map, and edit signature
            if (auto cmd = std::get_if<AddComponentCommand>(&entityCommand->action))
            {
                edit->second.signature.set(cmd->component, true);
                edit->second.componentsToInsert.insert_or_assign(cmd->component, cmd->data);
            }

            // Remove component from the map, and edit signature
            if (auto cmd = std::get_if<RemoveComponentCommand>(&entityCommand->action))
            {
                edit->second.signature.set(cmd->component, false);
                edit->second.componentsToInsert.erase(cmd->component);
            }
        }
    }

    // Properly apply `Entity` events
    for (auto &[entity, edit] : entityEdits)
    {
        // If we despawn the entity
        if (edit.despawning)
        {
            Archetype &previous = getArchetype(entity);

            // Request transfer to no `Archetype` (destruction of components & removal of space for entity)
            previous.requestTransfer(entity, nullptr);

            // Notify queries about the entity's destruction
            for (auto &[_, query] : m_queries)
                query->entityDeleted(entity);

            m_entityLocations.erase(entity);

            // Make `Entity` available again
            m_availableEntities.push(entity);

            continue;
        }

        // Get the new `Archetype` from its `Signature`
        auto &newArchetype = getArchetype(edit.signature);
        // Get or create
        EntityLocation &location = m_entityLocations[entity];

        // If the entity has just spawned, allocate it manually
        if (edit.justSpawned)
            location.row = newArchetype.allocateEntity(entity);
        // Otherwise, request transfer from the old archetype to the new
        else
            location.row = m_archetypes.find(m_entityLocations[entity].signature)->second.requestTransfer(entity, &newArchetype);

        // Then, transfer all new components (if there are any)
        // SAFETY : Safe because we allocated a row for the `Entity` the right way
        for (auto [id, data] : edit.componentsToInsert)
            newArchetype.transferComponentUnsafe(id, location.row, data.get());

        // Update signature
        location.signature = edit.signature;

        // Now that our entity is updated, notify all queries about it
        for (auto &[_, query] : m_queries)
            query->entityUpdated(entity, location.signature, edit.signature, newArchetype);
    }

    // Clear command queue for next tick
    m_commandQueue.clear();

    // End tick
    m_ticking = false;
}

IColumn *World::getTemplateColumn(ComponentID id) const
{
    return m_templateColumns[id]->clone();
}

inline EntityLocation &World::entityLocation(Entity entity)
{
    return m_entityLocations[entity];
}

inline const EntityLocation &World::entityLocation(Entity entity) const
{
    return m_entityLocations.find(entity)->second;
}