#pragma once
#include <unordered_map>
#include <unordered_set>
#include <bits/unique_ptr.h>
#include <bits/shared_ptr.h>
#include <typeindex>
#include <queue>

#include "query/Query.hpp"
#include "CommandsImpl.hpp"
#include "Global.hpp"
#include "System.hpp"
#include "Archetype.hpp"
#include "Commands.hpp"
#include "Resource.hpp"

using WorldID = std::uint8_t;

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
    static inline std::unordered_set<std::shared_ptr<World>> s_Worlds;      //!< All existing `World` instances. Prevents them from being moved in memory, and thus invalidating references to them
    std::unordered_map<std::type_index, std::shared_ptr<void>> m_resources; //!< Data of all `Resource`s inserted into the `World`
    ComponentID m_registeredComponents = 0;                                 //!< Number of registered components in the `World`. Used for pseudo type-checking at runtime.
    mutable ComponentID m_nextComponentID = 0;                              //!< The next registered component will have this ID. After creation of the `World`, should be the same as `m_registeredComponents`
    std::unordered_map<Signature, Archetype> m_archetypes;                  //!< All `Archetypes` created within this `World`
    std::unordered_map<Entity, EntityLocation> m_entityLocations;           //!< Maps entities to the `Archetype` in which they are stored
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
    std::vector<AnyCommand> m_commandQueue;                                 //!< The queue of actions to apply. Implemented as a `vector` for iteration & clearing efficiency
    std::vector<std::shared_ptr<ISystem>> m_systems;                        //!< Contains all `System`s in this `World`, type erased
    std::queue<Entity> m_availableEntities;                                 //!< Contains all valid entities that are currently not spawned
    std::vector<IColumn *> m_templateColumns;                               //!< Contains 1 empty `Column` of each registered component type, for `Archetype` instantiation
    bool m_ticking{false};                                                  //!< Is a world tick currently running ?

    /**
     * @brief Get the `ComponentID` of a specific component type, without checking if it has been registered
     *
     * @return ComponentID
     */
    template <typename C>
    ComponentID getComponentIDUnchecked() const;

    /**
     * @brief Returns an Entity ID, and considers it spawned.
     *
     * @return Entity
     */
    Entity reserveEntity();

    /**
     * @brief Get an `Achetype` reference from the type of the components it contains
     *
     * Will create the `Archetype` if it doesn't exist
     *
     * @tparam Cs Types of the components. Specifiers & references are ignored
     * @return ArchetypePtr
     */
    template <typename... Cs>
    Archetype &getArchetype();

    /**
     * @brief Get an `Archetype`reference from its signature
     *
     * @copydetails World::getArchetype()
     *
     * @param sig
     * @return ArchetypePtr
     */
    Archetype &getArchetype(const Signature &sig);

    /**
     * @brief Get the `Archetype` holding a specific `Entity`
     *
     * @param entity
     * @return Archetype&
     */
    Archetype &getArchetype(Entity entity);

    /**
     * @brief Builds a system param from its type and returns it
     *
     * @tparam Param
     * @return auto
     */
    template <IsSystemParam Param>
    Param buildSystemParam();

    /**
     * @brief Inserts a resource into the `World`, and returns a handle to it.
     *
     * For a given type `T`, there can only be 1 instance of a `Resource<T>` per `World`
     *
     * @tparam Res
     * @param res
     * @return Resource<Res>
     */
    template <typename Res>
    Resource<std::decay_t<Res>> insertResource(Res &&res);

    /** @brief Creates a new `World`. Private constructor */
    World();

    /** @brief Prevents `World`s from being moved */
    World(World &&) = delete;

    // /** @brief `World`s are externally non-movable, for reference invalidation reasons. */
    // World(World &&) = default;
    // World(World &) = default;

public:
    friend Commands;
    friend Archetype;
    friend class EntityCommands;

    ~World() = default;

    /**
     * @brief Creates a new `World` and makes the following components available in it.
     *
     * @tparam AllComponents Components that will be registered in the world
     * @return World&
     */
    template <typename... AllComponents>
    static World &Create();

    /**
     * @brief Builds a `Signature` from the given component types
     *
     * @tparam Cs Component types to include in the `Signature`
     * @return Signature
     */
    template <typename... Cs>
    Signature buildSignature() const;

    /**
     * @brief Returns a `CommandQueue` allowing modification of this `World`
     *
     * @return CommandQueue
     */
    Commands commands();

    /**
     * @brief Spawns a new entity into the world
     *
     * This `Entity` is effectively created at the end of the current (or next if no tick is currently running) `World` tick, but commands can still be queued to add or remove components from the `Entity`, or even despawn it before it even spawned.
     *
     * @return EntityCommands Commands to edit the newly created `Entity`
     */
    EntityCommands spawn();

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
     * @brief Adds a single `System` to the `World`, by creating it from a function with the right type signature
     *
     * This function must return void, and only take parameters satisfying the `IsSystemParam` concept.
     *
     * @tparam Params Type of the parameters of the function
     * @param system
     */
    template <IsSystemParam... Params>
    void addSystem(void (*system)(Params...));

    /**
     * @brief Adds multiple `System`s to the `World`, by creating them from a function with the right type signature.
     *
     * @copydetails World::addSystem()
     *
     * @tparam Funcs The type parameters for the functions
     * @param systems The underlying functions of the `System`s
     */
    template <IsSystemFunction... Funcs>
    void addSystems(Funcs... systems);

    /**
     * @brief Inserts `Resource`s into the `World`, and returns handles to them
     *
     * @copydetails World::insertResource()
     *
     * @tparam Res The resource types
     * @param res
     * @return std::tuple<Resource<Res>...>
     */
    template <typename... Res>
    std::tuple<Resource<std::decay_t<Res>>...> insertResources(Res &&...res);

    /**
     * @brief Get the `Resource` of given type if it was inserted in the map
     *
     * @tparam Res
     * @return Resource<Res>
     */
    template <typename Res>
    Resource<Res> getResource() const;

    /**
     * @brief Get the `ComponentID` of a component type
     *
     * @warning Throws an error if the component type was not registered yet
     *
     * @tparam C
     * @return ComponentID
     */
    template <typename C>
    ComponentID getComponentID() const;

    /**
     * @brief Get a new empty `Column` holding the specified component
     * 
     * @param id
     * @return IColumn* 
     */
    IColumn* getTemplateColumn(ComponentID id) const;

    /**
     * @brief Returns an `Entity`'s `EntityLocation`, giving information on which `Archetype` and `Row` it is stored in.
     * 
     * @param entity 
     * @return EntityLocation 
     */
    EntityLocation& entityLocation(Entity entity);
    
    /**
     * @copybrief  EntityLocation& World::entityLocation()
     * 
     * @param entity 
     * @return const EntityLocation& 
     */
    const EntityLocation& entityLocation(Entity entity) const;
};
