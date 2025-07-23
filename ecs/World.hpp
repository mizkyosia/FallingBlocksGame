#pragma once
#include <unordered_map>
#include <bits/unique_ptr.h>
#include <bits/shared_ptr.h>
#include <typeindex>

#include "Commands.hpp"
#include "Archetype.hpp"
#include "query/Query.hpp"
#include "System.hpp"

/**
 * @brief An ECS World
 *
 * Contains all entities, components, systems & other resources inserted by the user.
 *
 *
 */
class World
{
private:
    std::unordered_map<std::type_index, ArchetypePtr> m_archetypes; //!< All `Archetypes` created within this `World`
    /**
     * @cond TURN_OFF_DOXYGEN
     * Using a `vector` to contain the queries + an unordered_map would mean faster iteration speed
     * when updating those queries when entities are destroyed or modified, but the tradeoff would be
     * to update `System` parameters each time a `Query` is created, which could potentially be worse in performance
     * Besides, queries are often too large to be efficiently loaded in the cache. And without proper caching,
     * iterating on a hashmap vs on a vector is not that different performance-wise
     * @endcond
     */
    std::unordered_map<std::type_index, std::shared_ptr<IQuery>> m_queries; //!< Maps a `Query` type to its data.
    std::vector<std::shared_ptr<ISystem>> m_systems;                        //!< Contains all `System`s in this `World`, type erased
    Commands m_commandQueue;                                                //!< The `CommandQueue` pertaining to this `World`. Allows modifications of the `World` through `System`s
    bool m_ticking;                                                         //!< Is a world tick currently running ?

public:
    World(/* args */);
    ~World() = default;

    /**
     * @brief Returns the `CommandQueue` of this `World`
     *
     * @return CommandQueue&
     */
    Commands &commands();

    /**
     * @brief Spawns a new entity into the world
     *
     * @return Entity The newly created entity's ID
     */
    Entity spawn();

    /**
     * @brief Advance the `World` by one tick, if it isn't already in the middle of one.
     *
     * Runs all systems, and apply all deferred changes to the world at the end of the tick.
     *
     */
    void tick();

    /**
     * @brief Gets a`Query` from its parameters
     *
     * Creates the `Query` if it doesn't exist
     *
     * @tparam Filter Optional `QueryFilter` for the `Query`
     * @tparam Params Parameters that are to be fetched in the query. Look up `Query` to learn more about those parameter types
     * @return Query<Filter, Params...>&
     */
    template <IsQueryFilter Filter = None, typename... Params>
    Query<Filter, Params...> &query();

    /**
     * @brief Adds multiple `System`s to the `World`, by creating them from a function with the right type signature.
     *
     * This function must return void, and only take parameters satisfying the `IsSystemParam` concept.
     *
     * @tparam Funcs The type parameters for the functions
     * @param systems The underlying functions of the `System`s
     */
    template <IsSystemFunction... Funcs>
    void addSystems(Funcs... systems);

    /**
     * @brief Registers component types in the `World`, making them able to be attached to entities
     *
     * @tparam Cs Types of the components
     */
    template <typename... Cs>
    void registerComponents();
};
