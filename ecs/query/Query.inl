#pragma once
#include "Query.hpp"
#include "../Archetype.hpp"
#include "../World.hpp"

#include <boost/core/demangle.hpp>

template <IsQueryFilter Filter, typename... Data>
template <typename T>
inline auto Query<Filter, Data...>::fetchData(Entity entity, std::shared_ptr<IArchetype> newArchetype)
{
    if constexpr (IsSpecializationOf<T, Has>)
    {
        // Check for availability of component
        return newArchetype->get<GetFirstParam<T>>(entity) != nullptr;
    }
    else if constexpr (IsSpecializationOf<T, Maybe>)
    {
        // Fetching a component that may not exist, nullptr is allowed
        return newArchetype->get<GetFirstParam<T>>(entity);
    }
    else if constexpr (std::is_same_v<T, EntityCommands>)
    {
        // Fetch an `EntityCommands` instance
        return world.commands().entity(entity);
    }
    else
    {
        // Most generic case, we're fetching a component that should exist
        T *comp = newArchetype->get<T>(entity);
        // If it does not, we fucked up somewhere earlier
        if (comp == nullptr)
            throw std::runtime_error{std::string{"Internal error : Entity n°"} + std::to_string(entity) + " guaranteed to have component " + boost::core::demangle(typeid(T).name()) + " but does not have one"};
        return comp;
    }
}

template <IsQueryFilter Filter, typename... Data>
inline auto Query<Filter, Data...>::fetchDataRow(Entity entity, std::shared_ptr<IArchetype> newArchetype)
{
    // Returns a new `Row` for the requested entity
    return std::make_tuple(fetchData<Data>(entity)...);
}

template <IsQueryFilter Filter, typename... Data>
inline void Query<Filter, Data...>::entityUpdated(const Entity id, const Signature &previous, const Signature &current, std::shared_ptr<IArchetype> newArchetype)
{
    bool match = ((current & signature) == signature) && filter.fetch(current, world);
    auto it = entityToIndex.find(id);

    // If the query has selected the entity
    if (match)
    {
        auto row = fetchDataRow(id);
        // Insert it if needed
        if (it == entityToIndex.end())
        {
            entityToIndex[id] = data.size();
            data.push_back(row);
        }
        // Or simply update it
        else
            data[it->second] = row;
    }
    // Else, remove it if it was selected before
    else if (it != entityToIndex.end())
        entityDeleted(id);
}

template <IsQueryFilter Filter, typename... Data>
inline void Query<Filter, Data...>::entityDeleted(const Entity id)
{
    // Check if the query contains the entity
    auto it = entityToIndex.find(id);
    if (it == entityToIndex.end())
        return;

    size_t index = it->second;
    size_t last = data.size() - 1;

    // Swap operation to put the removed entity at the back of the data vector, to pop it rather than erase + move vector contents
    // Even with the map iteration, it is faster than `data.erase()` in most cases
    if (index != last)
    {
        data[index] = std::move(data[last]);
        for (auto &[eid, idx] : entityToIndex)
        {
            if (idx == last)
            {
                idx = index;
                break;
            }
        }
    }

    // And pop the last element
    data.pop_back();
    entityToIndex.erase(it);
}

template <IsQueryFilter Filter, typename... Data>
inline const Query<Filter, Data...>::Row &Query<Filter, Data...>::operator[](Entity entity)
{
    auto idx = entityToIndex.find(entity);
    if (idx == entityToIndex.end())
        throw std::runtime_error{"The entity N°" + std::to_string(entity) + " does not match the following query : " + boost::core::demangle(typeid(Query<Filter, Data...>).name())};

    return data[*idx];
}

template <IsQueryFilter Filter, typename... Data>
inline bool Query<Filter, Data...>::captured(Entity entity)
{
    return entityToIndex.contains(entity);
}

template <IsQueryFilter Filter, typename... Data>
inline Query<Filter, Data...>::Query(World &world) : filter(), data(), entityToIndex(), signature(0), world(world)
{
    // Fold expression to set the signature
    (_setSignature<Data>(), ...);
}

template <IsQueryFilter Filter, typename... Data>
template <typename T>
inline void Query<Filter, Data...>::_setSignature()
{
    if constexpr (!IsSpecializationOf<T, Has> && !IsSpecializationOf<T, Maybe> && !std::is_same_v<T, EntityCommands>)
    {
        signature.set(world.getComponentID<T>());
    }
}