#pragma once
#include "Archetype.hpp"
#include "World.hpp"

#include <type_traits>

template <typename T>
inline T *IArchetype::get(Entity entity)
{
    // Safety : the pointer is guaranteed by implementation to be a pointer of the requested type, cast as void for type erasure
    return static_cast<T *>(getComponentUnsafe(m_world.getComponentID<T>(), entity));
}

template <typename... Components>
inline Archetype<Components...>::Archetype(World &world) : m_signature(0), m_world(world)
{
    // Create signature
    (m_signature.set(world.getComponentID<Component>()), ...);
}

template <typename... Components>
template <typename Component>
inline std::vector<Component> &Archetype<Components...>::getColumn()
{
    return std::get<Component>(m_table);
}

template <typename... Components>
template <typename Component>
inline void Archetype<Components...>::set(Entity entity, Component component)
{
    auto it = m_entityToIndex.find(entity);
    if (it == m_entityToIndex.end())
        return;

    if constexpr (InPack<Component, Components...>)
    {
        std::get<Component>(m_table)[*it] = component;
    }
}

template <typename... Components>
inline void Archetype<Components...>::transferComponentUnsafe(ComponentID component, size_t index, void *data)
{
    //!!! Safety : the index MUST come from `Archetype::allocateEntity()`

    (..., [&]<typename T>()
          {
        if (m_world.getComponentID<T>() == component) {
            // Cast data as right type then dereference & move value
            std::get<T>(m_table)[index] = *static_cast<T*>(data);
        } }.template operator()<Components>());
}

template <typename... Components>
inline size_t Archetype<Components...>::allocateEntity(Entity entity)
{
    if (m_entityToIndex.contains(entity))
        return;

    size_t idx = SIZE_MAX;

    // Fold expression to iterate over types
    ([&]<typename T>()
     {
         // Resize each column and increase their size by 1
        auto &col = std::get<T>(m_table);
        auto size = col.size();

        if(idx == SIZE_MAX) idx = size;
        // Normally impossible, but just in case
        else if(idx != size)
            throw std::runtime_error{"ECS error : Differently sized columns in archetype : " + boost::core::demangle(typeid(Archetype<Components...>).name())};
        
        col.resize(col.size() + 1); }.template operator()<Components>(),
     ...);

    m_entityToIndex.insert({entity, idx});

    return idx;
}

template <typename... Components>
inline void Archetype<Components...>::requestTransfer(Entity entity, IArchetype *to)
{
    // Allocate space for the entity's components
    size_t newIndex = to->allocateEntity(entity);
    size_t idx = m_entityToIndex[entity];

    size_t last = idx;

    // Transfer all components & erase them from this `Archetype`
    (..., [&]<typename T>()
          {
            auto &col = std::get<T>(m_table);
            to->transferComponentUnsafe(m_world.getComponentID<T>(), newIndex, static_cast<void *>(&std::get<T>(m_table)[idx]));
            // Component transferred, we can now remove it from here (via swap & pop)
            last = col.size() - 1;
            if(idx != last) 
                col[idx] = std::move(col[last]);
            col.pop_back(); }.template operator()<Components>());

    // If it already was the last, no need to re-map index
    if (idx != last)
    {
        for (auto &pair : m_entityToIndex)
        {
            if (pair.second == last)
                pair.second = idx;
        }
    }
}

template <typename... Components>
inline void *Archetype<Components...>::getComponentUnsafe(ComponentID component, Entity entity)
{
    auto column = m_componentToIndex.find(component);
    auto row = m_entityToIndex.find(entity);

    if (column == m_componentToIndex.end() || row == m_entityToIndex.end())
        return nullptr;
    return static_cast<void *>(&m_table[*column][*row]);
}

template <typename... Components>
inline bool Archetype<Components...>::hasEntity(Entity entity)
{
    return m_entityToIndex.contains(entity);
}

template <typename... Components>
inline bool Archetype<Components...>::match(const Signature &sig)
{
    // Is this signature a subset of the given signature ?
    return (sig & m_signature) == m_signature;
}
