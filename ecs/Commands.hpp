#pragma once
#include "Global.hpp"
#include <queue>
#include <functional>
#include <variant>

class EntityCommands;

/**
 * @brief Allow queuing of commands to edit the `World`
 *
 * These commands include :
 *
 * - Spawning/destroying an `Entity`
 * - Adding/removing/fetching components of an `Entity`
 * - Fetching and inserting `Resource`s
 * - Adding `System`s
 *
 * @attention Note that as all commands passed are queued, they are only applied at the end of the current tick. The only exception to that rule is fetching components through `EntityCommands`
 */
class Commands
{
private:
    World &m_world; //!< The `World` this queue has access to

    Commands(World &world);

public:
    ~Commands() = default;

    friend World;

    /**
     * @brief Spawns a new entity into the world
     *
     * @tparam Components Types of the components to attach to the entity at its creation
     * @param components Components that will be given to the entity once it is created
     * @return Entity
     */
    template <typename... Components>
    EntityCommands spawn(Components... components);

    /**
     * @brief Returns an `EntityCommands` for modifying a specific entity, rather than the entire `World`
     *
     * @return EntityCommandQueue
     */
    EntityCommands entity(Entity entity);
};

/**
 * @brief Gives mutable access to an entity's data. Allows for removing/adding components, or destruction of the entity
 *
 */
class EntityCommands
{
    Entity m_entity;
    World *m_world;

    EntityCommands(World *world, Entity entity);

public:
    ~EntityCommands() = default;
    EntityCommands(const EntityCommands &other);

    EntityCommands& operator=(const EntityCommands& other);

    friend Commands;
    friend World;

    /**
     * @brief Get the component of the given `type` if the entity has one, or `nullptr` otherwise
     *
     * @tparam Component
     * @return Component*
     */
    template <typename Component>
    Component *getComponent();

    /**
     * @brief Gets the `Entity` associated with this command
     *
     * @return Entity
     */
    Entity entity();

    /**
     * @brief Tries to insert a given component to the entity. If another component of the same type exists linked to this entity, this will overwrite it
     *
     * @warning Will throw an error if the given component type is not registered in the `World`
     *
     * @tparam Component
     */
    template <typename Component>
    void insertComponent(Component component);

    /**
     * @brief Removes a component from the entity
     *
     * @warning Will throw an error if the given component type is not registered in the `World`
     *
     * @tparam Component
     */
    template <typename Component>
    void removeComponent();

    /**
     * @brief Destroys the entity, and all components associated with it.
     *
     */
    void destroy();
};
