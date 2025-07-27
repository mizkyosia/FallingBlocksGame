#pragma once
#include "Archetype.hpp"
#include "World.hpp"
#include "Column.hpp"

#include <boost/core/demangle.hpp>
#include <type_traits>
#include <iostream>

template <typename T>
inline T *Archetype::get(Entity entity)
{
    // Safety : the pointer is guaranteed by implementation to be a pointer of the requested type, cast as void for type erasure
    return static_cast<T *>(getComponentUnsafe(m_world.getComponentID<T>(), entity));
}

inline Signature Archetype::getSignature()
{
    return m_signature;
}

template <typename Component>
inline Column<Component> &Archetype::getColumn()
{
    return m_table[m_componentToIndex[m_world.getComponentID<Component>()]];
}

template <typename Component>
inline void Archetype::set(Entity entity, Component component)
{
    auto loc = m_world.entityLocation(entity);
    if (loc.signature != m_signature)
        return;

    getColumn<Component>()->set(loc.row, std::move(component));
}

inline void Archetype::transferComponentUnsafe(ComponentID component, size_t index, void *data)
{
    //!!! Safety : the index MUST come from `Archetype::allocateEntity()`

    // Cast data as right type then dereference & move value
    m_table[m_componentToIndex[component]]->setComponentUnsafe(data, index);
}

inline size_t Archetype::requestTransfer(Entity entity, Archetype *to)
{
    // Allocate space for the entity's components (if needed)
    size_t newIndex = to == nullptr ? 0 : to->allocateEntity(entity);
    auto loc = m_world.entityLocation(entity);

    size_t last = loc.row;

    // Transfer all components & erase them from this `Archetype`
    for (auto &col : m_table)
    {

        // If we actually need to do a transfer
        if (to != nullptr)
            to->transferComponentUnsafe(col->componentID, newIndex, col->get(loc.row));

        // Component transferred, we can now remove it from here (via swap & pop)
        last = col->size() - 1;
        col->erase(loc.row);
    }

    // If it already was the last, no need to re-map index
    if (loc.row != last)
    {
        for (auto &pair : m_world.m_entityLocations)
        {
            if (pair.second.row == last)
                pair.second.row = loc.row;
        }
    }

    return newIndex;
}

inline void *Archetype::getComponentUnsafe(ComponentID component, Entity entity)
{
    auto column = m_componentToIndex.find(component);
    auto location = m_world.entityLocation(entity);

    if (column == m_componentToIndex.end() || location.signature != m_signature)
        return nullptr;
    return m_table[column->second]->get(location.row);
}

inline bool Archetype::hasEntity(Entity entity)
{
    return m_world.entityLocation(entity).signature == m_signature;
}

inline bool Archetype::match(const Signature &sig)
{
    // Is this signature a subset of the given signature ?
    return (sig & m_signature) == m_signature;
}

template <typename Component>
inline void Archetype::insertColumn(Column<Component> *col)
{
    m_componentToIndex[m_world.getComponentID<Component>()] = m_table.size();
    m_table.push_back(col);
};