#pragma once
#include "Filter.hpp"
#include "World.hpp"

template <IsQueryFilter... Filters>
CompositeFilter<Filters...>::CompositeFilter() : filters{}
{
    // Init all filters that are to be used later
    (filters.push_back(std::make_unique<Filters>()), ...);
}

inline bool None::fetch(const Signature &sig, const World &world)
{
    return true;
}

template <typename... Components>
bool With<Components...>::fetch(const Signature &sig, const World &world)
{
    {
        return (... && sig[world.getComponentID<Components>()]);
    }
}

template <typename... Components>
bool Without<Components...>::fetch(const Signature &sig, const World &world)
{
    {
        return (... && !sig[world.getComponentID<Components>()]);
    }
}

template <IsQueryFilter... Filters>
bool Any<Filters...>::fetch(const Signature &sig, const World &world)
{
    for (auto &filter : this->filters)
        if (filter->fetch(sig, world))
            return true;
    return false;
}

template <IsQueryFilter... Filters>
bool One<Filters...>::fetch(const Signature &sig, const World &world)
{
    // Checks that exacty one of those filters is true
    bool one = false;
    for (auto &filter : this->filters)
        if (filter->fetch(sig, world))
        {
            if (one)
                return false;
            one = true;
        }

    return one;
}

template <IsQueryFilter... Filters>
bool All<Filters...>::fetch(const Signature &sig, const World &world)
{
    for (auto &filter : this->filters)
        if (!filter->fetch(sig, world))
            return false;
    return true;
}