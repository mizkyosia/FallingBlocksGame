#pragma once
#include <Global.hpp>
#include <vector>
#include <bits/unique_ptr.h>

/** \brief Virtual base for all query filters */
struct QueryFilter
{
    virtual ~QueryFilter() = default;

    /** \brief Required function. For the given entity signature, returns "true" if the entity matches the filter*/
    virtual bool fetch(const Signature &sig) = 0;
};

/** \brief Asserts that template arguments are derived from `QueryFilter` */
template <typename T>
concept IsQueryFilter = std::derived_from<T, QueryFilter>;

/** \brief Empty struct representing no filter. Used for placeholding purposes */
struct None : public QueryFilter
{
    bool fetch(const Signature &sig) override
    {
        return true;
    }
};

/** \brief Query filter matching all entities containing all the given components, without fetching said components */
template <typename... _Components>
struct With : public QueryFilter
{
    bool fetch(const Signature &sig) override { return (... && signature[get_unique_component_id()]); }
};

/** \brief Query filter matching all entities containing none of the given components */
template <typename... _Components>
struct Without : public QueryFilter
{
    bool fetch(const Signature &sig) override { return (... && !signature[get_unique_component_id()]); }
};

/** \brief Struct used for compositing filters. Virtual. Used to not repeat boilerplate code */
template <IsQueryFilter... _Filters>
struct CompositeFilter : public QueryFilter
{
    /** \brief The filters to compose */
    std::vector<std::unique_ptr<QueryFilter>> filters;

    CompositeFilter() : filters{}
    {
        // Init all filters that are to be used later
        (filters.push_back(std::make_unique<_Filters>()), ...);
    }
};

/**
 * \brief Query filter compositing multiple query filters, and evaluating to `true` if any of those filters evaluate to `true`
 * \tparam _Filters The filters to compose. Must be derived from `QueryFilter`
 * */
template <IsQueryFilter... _Filters>
struct Any : public CompositeFilter<_Filters...>
{
    bool fetch(const Signature &sig) override
    {
        for (auto &filter : filters)
            if (filter->fetch(sig))
                return true;
        return false;
    }
};

/**
 * \brief Query filter compositing multiple filters, and asserts that only one of them is true
 * \tparam _Filters The filters to compose. Must be derived from `QueryFilter`
 * */
template <IsQueryFilter... _Filters>
struct One : public CompositeFilter<_Filters...>
{
    bool fetch(const Signature &sig) override
    {
        // Checks that exacty one of those filters is true
        bool one = false;
        for (auto &filter : filters)
            if (filter->fetch(sig))
            {
                if (one)
                    return false;
                one = true;
            }

        return one;
    }
};

/**
 * \brief Query filter compositing multiple filters, and asserts that all of them are true
 * \tparam _Filters The filters to compose. Must be derived from `QueryFilter`
 */
template <IsQueryFilter... _Filters>
struct All : public CompositeFilter<_Filters...>
{
    bool fetch(const Signature &sig) override
    {
        for (auto &filter : filters)
            if (!filter->fetch(sig))
                return false;
        return true;
    }
};

// ============================== DEFERRED FILTERS : MIGHT BE IMPLEMENTED LATER ==============================

/**
 * @brief Virtual "deferred" filter implementation
 *        
 * A "deferred" filter is a filter that basically detects changes between two ticks, for example a component being added/removed. 
 * 
 * Be careful though, using them can slow down the program, as queries containing them need to update their data 2x as much as regular queries
 * 
 * @tparam Ts
 */
template <typename... Ts>
struct DeferredFilter
{
};

/**
 * @brief Selects only entities to which a component of the given type was added on the last tick. This can include entities that are by default created with this component
 * 
 * @tparam Components 
 */
template<typename Component>
struct Added : public DeferredFilter<Component> {
};

/**
 * @brief Selects only entities to which a component of the given type was removed on the last tick.
 * 
 * @tparam Components 
 */
template<typename Component>
struct Added : public DeferredFilter<Component> {
};