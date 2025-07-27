#pragma once
#include <boost/core/demangle.hpp>
#include <tuple>

#include "World.hpp"
#include "query/Query.hpp"

template <typename C>
ComponentID World::getComponentIDUnchecked() const
{
    static ComponentID id = m_nextComponentID++;
    return id;
}

template <typename... Cs>
inline Archetype &World::getArchetype()
{
    return getArchetype(buildSignature<Cs...>());
}

template <IsSystemParam Param>
inline Param World::buildSystemParam()
{
    if constexpr (IsWorldRef<Param>)
    {
        return static_cast<World &>(*this);
    }
    else if constexpr (IsCommands<Param>)
    {
        return commands();
    }
    else if constexpr (traits::QueryTraits<std::remove_reference_t<Param>>::is_query)
    {
        using Filter = typename traits::QueryTraits<std::remove_reference_t<Param>>::Filter;
        using DataTuple = typename traits::QueryTraits<std::remove_reference_t<Param>>::DataTuple;

        return [&]<typename... Data>(std::tuple<Data...> *) -> decltype(auto)
        {
            return query<Filter, Data...>();
        }(static_cast<DataTuple *>(nullptr));
    }
    else if constexpr (IsResource<Param>)
    {
        return getResource<typename traits::resource_type<Param>::type>();
    }
    else
    {
        static_assert(false, "Invalid systemparam in registered system");
    }
}

template <typename C>
inline ComponentID World::getComponentID() const
{
    ComponentID id = getComponentIDUnchecked<std::remove_cvref_t<C>>();
    if (id >= m_registeredComponents)
        throw std::runtime_error{"ECS Error : The following component type was not registered at world instantiation : " + boost::core::demangle(typeid(C).name())};
    return id;
}

template <typename... Cs>
inline Signature World::buildSignature() const
{
    Signature sig = 0;
    (sig.set(getComponentID<Cs>), ...);
    return sig;
}

template <typename... AllComponents>
inline World &World::Create()
{
    // Create the world
    auto world = std::shared_ptr<World>(new World());

    // Register all components
    ((world->m_registeredComponents = world->getComponentIDUnchecked<std::remove_cvref_t<AllComponents>>()), ...);

    world->m_registeredComponents++;

    if (world->m_registeredComponents != world->m_nextComponentID)
        throw std::runtime_error{"ECS Error : Difference between registered components & number of component IDs at instantiation"};

    // Instantiate our template columns
    world->m_templateColumns.reserve(sizeof...(AllComponents));
    (world->m_templateColumns.push_back(new Column<AllComponents>(*world)), ...);

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

    // Insert it into the world
    m_queries.insert({std::type_index(typeid(Query<Filter, Params...>)),
                      newQuery});

    // And instantiate it by checking all entities that match
    for (auto &[entity, location] : m_entityLocations)
        newQuery->entityUpdated(entity, location.signature, location.signature, m_archetypes.find(location.signature)->second);

    return *newQuery;
}

template <IsSystemParam... Params>
inline void World::addSystem(void (*system)(Params...))
{
    auto params = std::tuple<Params...>{
        buildSystemParam<Params>()...};
    m_systems.push_back(std::make_shared<System<Params...>>(params, system));
}

template <IsSystemFunction... Funcs>
inline void World::addSystems(Funcs... systems)
{

    ([&]
     {
        using ParamsTuple = typename traits::system_state<Funcs>::type;
        [&]<typename... Params>(std::tuple<Params...>*) {
            return addSystem<Params...>(systems);
        }(static_cast<ParamsTuple*>(nullptr)); }(), ...);
}

template <typename Res>
inline Resource<std::decay_t<Res>> World::insertResource(Res &&res)
{
    using BareType = std::decay_t<Res>;

    auto ptr = std::make_shared<BareType>(std::forward<Res>(res));
    m_resources.insert_or_assign(std::type_index(typeid(BareType)),
                                 std::shared_ptr<void>(ptr));
    return Resource<BareType>(ptr);
}

template <typename... Res>
inline std::tuple<Resource<std::decay_t<Res>>...> World::insertResources(Res &&...res)
{
    return std::make_tuple(insertResource(std::forward<Res>(res))...);
}

template <typename Res>
inline Resource<Res> World::getResource() const
{
    return Resource<Res>(std::static_pointer_cast<Res>(m_resources.find(std::type_index(typeid(Res)))->second));
}