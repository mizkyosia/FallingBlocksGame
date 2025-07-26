#include "ECS.hpp"
#include <variant>

World::World() {};

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
    return EntityCommands(*this, e);
}

ArchetypePtr World::getArchetype(const Signature &sig)
{
    return ArchetypePtr();
}

void World::tick()
{
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
                edit->second.signature = m_entityToArchetype[entityCommand->entity]->getSignature();
            }

            // Add component to the map, and edit signature
            if (auto cmd = std::get_if<AddComponentCommand>(&entityCommand->action))
            {
                edit->second.signature.set(cmd->component);
                edit->second.componentsToInsert.insert_or_assign(cmd->component, cmd->data);
            }

            // Remove component from the map, and edit signature
            if (auto cmd = std::get_if<AddComponentCommand>(&entityCommand->action))
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
            // Request transfer to no `Archetype` (destruction of components & removal of space for entity)
            m_entityToArchetype[entity]->requestTransfer(entity, nullptr);

            // Unmap archetype from `Entity`
            m_entityToArchetype.erase(entity);

            // Notify queries about the entity's destruction
            for (auto &[_, query] : m_queries)
                query->entityDeleted(entity);

            // Make `Entity` available again
            m_availableEntities.push(entity);
        }

        // Get the new `Archetype` from its `Signature`
        auto newArchetype = getArchetype(edit.signature);
        size_t newRow = 0;

        // If the entity has just spawned, allocate it manually
        if (edit.justSpawned)
            newRow = newArchetype->allocateEntity(entity);
        // Otherwise, request transfer from the old archetype to the new
        else
            newRow = m_entityToArchetype[entity]->requestTransfer(entity, newArchetype.get());

        // Then, transfer all new components (if there are any)
        // SAFETY : Safe because we allocated a row for the `Entity` the right way
        for (auto [id, data] : edit.componentsToInsert)
            newArchetype->transferComponentUnsafe(id, newRow, data.get());

        // Now that our entity is updated, notify all queries about it
        for (auto &[_, query] : m_queries)
            query->entityUpdated(entity, edit.justSpawned ? 0 : m_archetypes[entity]->m_signature, edit.signature);

        // And finally, update its `Archetype`
        m_entityToArchetype.insert_or_assign(entity, newArchetype);
    }

    // Clear command queue for next tick
    m_commandQueue.clear();
}
