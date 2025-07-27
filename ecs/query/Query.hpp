#pragma once
#include "Filter.hpp"
#include "Data.hpp"
#include <unordered_map>
#include <bits/shared_ptr.h>

// Forward declaration
class IArchetype;
struct IQuery
{
    virtual void entityUpdated(const Entity id, const Signature &previous, const Signature &current, std::shared_ptr<IArchetype> newArchetype) = 0;
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
private:
    /**
     * @brief Fetches a given query `Data` for a given entity
     *
     * @tparam Data
     * @param entity A valid ID for an entity selected by this query
     */
    template <typename T>
    static decltype(auto) fetchData(World &world, Entity entity, std::shared_ptr<IArchetype> newArchetype);

    /**
     * @brief Called by the `World` when an entity is updated. The query then removes/adds the entity to its search space as needed
     *
     * @param id ID of the updated entity
     * @param previous Previous signature of the entity. In case the entity was just created, this is the same as `current`
     * @param current New signature of the entity after modifications
     */
    void entityUpdated(const Entity id, const Signature &previous, const Signature &current, std::shared_ptr<IArchetype> newArchetype) override;
    /**
     * @brief Called by the `World` whe an entity is destroyed. The query then removes the entity from its search space, if it was in it
     *
     * @param id ID of the deleted entity
     */
    void entityDeleted(const Entity id) override;

    template <typename T>
    void _setSignature();

public:
    /**
     * @brief Creates a new query associated with a system
     * @tparam Filter The query's filter. Used to check for components associated with the entity without fetching them. Can be a composite filter. Can be omitted to not add any filters
     * @tparam Components The type of the components that will be fetched
     */
    Query(World &world);

    /**
     * @brief
     * Type of the actual data stored in the `Query`. Each entity has a `Row` containing its fetched data
     *
     * Slightly different from the `Data` template parameter used to construct it, most notably all component types are replaced by pointers, preserving `const`ness as needed
     */
    using Row = std::tuple<QueryDataOutput<Data>...>;

private:
    /**
     * @brief Fetches an entire row of `Data` pertaining to a given entity.
     *
     * @param entity A valid ID for an entity selected by this query
     */
    Row fetchDataRow(Entity entity, std::shared_ptr<IArchetype> newArchetype);

    Filter filter;                                    //!< Filter used in the query. May be a composite filter
    Signature signature;                              //!< Signature of the query
    std::vector<Row> data;                            //!< Data stored in the query. Packed into a vector for quick iteration
    std::unordered_map<Entity, size_t> entityToIndex; //!< Maps an entity ID to an index in the `data` vector. Used when an entity changes
    World &world;                                     //!< The `World` this query is attached to

public:
    /** @brief The begin iterator of the query's data */
    const auto begin() const { return data.begin(); };
    /** @brief The end iterator of the query's data */
    const auto end() const { return data.end(); };
    size_t size() const { return data.size(); }

    /**
     * @brief Can be used to fetch data relative to a specific entity.
     *
     * @warning Throws an error if the `Query` doesn't contain the specified entity
     *
     * @param entity The entity for whom we need to fetch the data
     * @return const Row& A const pointer to the query `Row` data associated with this entity.
     */
    const Row &operator[](Entity entity);

    /**
     * @brief Has this `Query` captured the specified entity ?
     *
     * @param entity
     * @return true
     * @return false
     */
    bool captured(Entity entity);

    /** Declare `World` as friend for global access */
    friend World;
};

namespace traits
{
    // Base case : not a Query
    template <typename>
    struct QueryTraits
    {
        static constexpr bool is_query = false;
    };

    // Specialization for Query
    template <IsQueryFilter Filt, typename... Data>
    struct QueryTraits<Query<Filt, Data...>>
    {
        static constexpr bool is_query = true;
        using Filter = Filt;
        using DataTuple = std::tuple<Data...>;
    };
}

template <typename T>
concept IsQueryRef = std::is_reference_v<T> && IsSpecializationOf<std::remove_reference_t<T>, Query>;

template <typename T>
concept IsComponentParameter = !std::is_reference_v<T> && !std::is_pointer_v<T> && !IsSpecializationOf<T, Has> && !IsSpecializationOf<T, Maybe> && !std::is_same_v<T, EntityCommands>;
