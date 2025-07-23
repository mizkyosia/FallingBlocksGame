#pragma once
#include <query/Filter.hpp>
#include <query/Data.hpp>
#include <unordered_map>

struct IQuery
{
    virtual void entityUpdated(const Entity id, const Signature &previous, const Signature &current) = 0;
    virtual void entityDeleted(const Entity id) = 0;
};

/**
 * @brief A query searching for specific components on entities, and returning a `Row` containing those components for every entity matching the query.
 *
 * Here is a list of valid `QueryData` types. For the following list, any generic type `T` can be made `const, and you will get a corresponding `T*` (or `const T*`) when iterating over the query. For specific query data types, more details can be found in their documentation :
 *
 *  - An `Entity`
 *  - An `EntityCommands` command queue. Note that acessing any component through this handle will be less optimized than just fetching it through the query, as it will not be pre-fetched. Moreover, any removes/additions of an entities components, or even creation/destruction of entities, are not applied until the end of a frame, as they are queued.
 *  - Any type `T`  where `T` is a registered Component type
 *  - An `Has<T>` where `T` is a registered Component type. When iterating the query, is replaced by a `bool` indicating if the entity has this component or not
 *  - A `Maybe<T>`, which fetches the component if it is a registered Component type and the entity has it, or `std::nullopt` otherwise.
 *
 * Additionally, you can pass a `QueryFilter` to restrict the search area of the `Query` _without fetching components_. These filters are as follows :
 *
 *  - `None` : No filter. Used as a placeholder
 *  - `With<Ts...>` : An entity must have all requested components to be selected by this `Query`
 *  - `Without<Ts...>` : An entity must have none of the requested components to be selected by this `Query`
 *  - `Any<Fs...>` : Composes multiple filters. If any of those filters is true, this one is.
 *  - `All<Fs...>` : Composes multiple filters. If all of those filters are true, this one is.
 *  - `One<Fs...>` : Composes multiple filters. If _exactly one_ of thos filters is true, this one is.
 *
 * @tparam Filter
 * @tparam Data
 */
template <IsQueryFilter Filter = None, typename... Data>
struct Query : public IQuery
{
    /**
     * @brief
     * Type of the actual data stored in the `Query`. Each entity has a `Row` containing its fetched data
     *
     * Slightly different from the `Data` template parameter used to construct it, most notably all component types are replaced by pointers, preserving `const`ness as needed
     */
    using Row = std::tuple<decltype(fetchData<Data>(std::declval<Entity>()))...>;

private:
    /**
     * @brief Creates a new query associated with a system
     * @tparam Filter The query's filter. Used to check for components associated with the entity without fetching them. Can be a composite filter. Can be omitted to not add any filters
     * @tparam Components The type of the components that will be fetched
     */
    Query(World &world);

    /**
     * @brief Fetches a given query `Data` for a given entity
     *
     * @tparam Data
     * @param entity A valid ID for an entity selected by this query
     */
    template <typename T>
    auto fetchData(Entity entity);

    /**
     * @brief Fetches an entire row of `Data` pertaining to a given entity.
     *
     * @param entity A valid ID for an entity selected by this query
     */
    auto fetchDataRow(Entity entity);

    /**
     * @brief Called by the `World` when an entity is updated. The query then removes/adds the entity to its search space as needed
     *
     * @param id ID of the updated entity
     * @param previous Previous signature of the entity. In case the entity was just created, this is the same as `current`
     * @param current New signature of the entity after modifications
     */
    void entityUpdated(const Entity id, const Signature &previous, const Signature &current) override;
    /**
     * @brief Called by the `World` whe an entity is destroyed. The query then removes the entity from its search space, if it was in it
     *
     * @param id ID of the deleted entity
     */
    void entityDeleted(const Entity id) override;

public:
    /** The begin iterator of the query's data */
    const auto begin() { return data.begin(); };
    /** The end iterator of the query's data */
    const auto end() { return data.end(); };

    /**
     * @brief Can be used to fetch data relative to a specific entity.
     *
     * @param entity The entity for whom we need to fetch the data
     * @return const Row& A const reference to the query `Row` data associated with this entity.
     */
    const Row &operator[](Entity entity);

    /** Declare `World` as friend for global access */
    friend World;

private:
    Filter filter;                                    // Filter used in the query. May be a composite filter
    Signature signature;                              // Signature of the query
    std::vector<Row> data;                            // Data stored in the query. Packed into a vector for quick iteration
    std::unordered_map<Entity, size_t> entityToIndex; // Maps an entity ID to an index in the `data` vector. Used when an entity changes
    World &world;                                     // The `World` this query is attached to
};

template <IsQueryFilter Filter, typename... Data>
template <typename T>
inline auto Query<Filter, Data...>::fetchData(Entity entity)
{
    // If we check for ownership of a component
    if constexpr (IsSpecializationOf<T, Has>)
        return world.getStore<T>()->has(entity);
    // If we fetch an optional component
    else if constexpr (IsSpecializationOf<T, Maybe>)
        return Maybe(world.getStore<T>()->get(entity));
    // Else, we're fetching a component
    else
        return world.getStore<T>()->get(entity);
}

template <IsQueryFilter Filter, typename... Data>
inline auto Query<Filter, Data...>::fetchDataRow(Entity entity)
{
    // Returns a new `Row` for the requested entity
    return std::make_tuple(fetchData<Data>()...);
}

template <IsQueryFilter Filter, typename... Data>
inline void Query<Filter, Data...>::entityUpdated(const Entity id, const Signature &previous, const Signature &current)
{
    bool match = ((current & signature) == signature) && filter.fetch(sig);
    auto it = entityToIndex.find(id);

    // If the query has selected the entity
    if (match)
    {
        auto row = fetchDataRow(entity);
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
    return data[entityToIndex[entity]];
}

template <IsQueryFilter Filter, typename... Data>
inline Query<Filter, Data...>::Query(World &world) : filter(), data(), entityToIndex(), signature(0), world(world)
{
    // Fold expression to set the signature
    (if constexpr (!IsSpecializationOf<Data, Has> && !IsSpecializationOf<Data, Maybe>) { signature.set(world.getComponentID<Data>()); }, ...);
}

/** Assures that the given type is a reference to a `Query` */
template <typename T>
concept IsQueryRef = std::is_reference_v<T> && IsSpecializationOf<std::remove_reference_t<T>, Query>;

template <typename... Params>
struct get_query_params
{
    using type = Params...;
};