#pragma once
#include <boost/core/demangle.hpp>
#include "World.hpp"
#include "query/Query.hpp"

template <typename C>
ComponentID World::getComponentIDUnchecked()
{
    static ComponentID id = m_nextComponentID++;
    return id;
}

template <typename... Cs>
inline Archetype<Cs...> &World::getArchetype()
{
    return *static_cast<Archetype<Cs...> *>(getArchetype(buildSignature<Cs...>()).get());
}

template <IsSystemParam Param>
inline Param World::buildSystemParam()
{
    if constexpr (IsWorldRef<Param>)
    {
        return *this;
    }
    else if constexpr (IsCommands<Param>)
    {
        return commands();
    }
    else if constexpr (traits::QueryTraits<Param>::is_query)
    {
        using Filter = typename traits::QueryTraits<Param>::Filter;
        using DataTuple = typename traits::QueryTraits<Param>::DataTuple;

        return std::apply([]<typename... Data>(std::type_identity<std::tuple<Data...>>)
                          { return query<Filter, Data...>(); }, std::type_identity<DataTuple>{});
    }
}

template <typename C>
inline ComponentID World::getComponentID()
{
    ComponentID id = getComponentIDUnchecked<std::remove_cvref_t<C>>();
    if (id >= m_registeredComponents)
        throw std::runtime_error{"ECS Error : The following component type was not registered at world instantiation : " + boost::core::demangle(typeid(C).name())};
    return id;
}

template <typename... Cs>
inline Signature World::buildSignature()
{
    Signature sig = 0;
    (sig.set(getComponentID<Cs>), ...);
    return sig;
}

template <typename... AllComponents>
inline World &World::Create()
{
    // Create the world
    auto world = std::make_shared<World>(World());
    // Register all components
    ((world->m_registeredComponents = world->getComponentIDUnchecked<std::remove_cvref_t<AllComponents>>()), ...);

    world->m_registeredComponents++;

    if (world->m_registeredComponents != world->m_nextComponentID)
        throw std::runtime_error{"ECS Error : Difference between registered components & number of component IDs at instantiation"};

    // Insert it into the register of `World`s
    s_Worlds.insert(world);
    // Return reference
    return *world;
}

template <IsQueryFilter Filter, typename... Params>
Query<Filter, Params...> &World::query()
{
    auto it = m_queries.find(std::type_index(typeid(Query<Filter, Params...>)));

    // If the right `Query` exists already
    if (it != m_queries.end())
        return *static_cast<Query<Filter, Params...> *>(it->second.get());

    // Otherwise, create it
    auto newQuery = std::make_shared<Query<Filter, Params...>>(*this);

    // And instantiate it by checking all entities that match
    for (auto &[entity, archetype] : m_entityToArchetype)
        newQuery->entityUpdated(entity, archetype->m_signature, archetype->m_signature);

    return *newQuery;
}

template <IsSystemParam... Params>
inline void World::addSystem(std::function<void(Params...)> system)
{
    auto params = std::make_tuple(buildSystemParam<Params>...);
    m_systems.push_back(std::make_shared<System<Params...>>(params, system));
}

template <IsSystemFunction... Funcs>
inline void World::addSystems(Funcs... systems)
{
    (addSystem<Funcs>(), ...);
}
