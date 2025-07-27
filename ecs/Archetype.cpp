#include "Archetype.hpp"
#include "World.hpp"
#include <iostream>

Archetype::Archetype(Archetype &&arch) : m_world(arch.m_world),
                                         m_table(std::move(arch.m_table)),
                                         m_componentToIndex(std::move(arch.m_componentToIndex)),
                                         m_signature(arch.m_signature)
{
    arch.m_table.clear();
    arch.m_signature = 0;
    arch.m_componentToIndex.clear();
}

Archetype::Archetype(World &world, Signature sig) : m_world(world), m_signature(sig)
{
    m_table.reserve(m_signature.count());

    for (int i = 0; i < m_signature.size(); i++)
        if (m_signature[i])
            m_table.push_back(m_world.getTemplateColumn(i));
}

Archetype::~Archetype()
{
    for (auto col : m_table)
    {
        delete col;
    }
}

size_t Archetype::allocateEntity(Entity entity)
{
    // Guaranteed to exist
    auto &loc = m_world.m_entityLocations[entity];

    if(m_signature == 0) return 0;

    // If already in `Archetype`
    if (loc.signature == m_signature)
        return loc.row;

    size_t idx = SIZE_MAX;

    // Fold expression to iterate over types
    for (auto &col : m_table)
    {
        // Resize each column and increase their size by 1
        auto size = col->size();

        if (idx == SIZE_MAX)
            idx = size;
        // Normally impossible, but just in case
        else if (idx != size)
            throw std::runtime_error{"ECS error : Differently sized columns in archetype : " + std::to_string(m_signature.to_ulong())};

        col->resize(col->size() + 1);
    }

    if (m_table.size() == 0)
        idx = 0;

    return idx;
}