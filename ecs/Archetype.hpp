#pragma once
#include <vector>
#include <unordered_map>
#include <type_traits>
#include <tuple>
#include <bits/unique_ptr.h>

#include "Global.hpp"

/**
 * @brief Base virtual class of all `Archetype`s. Used solely for type erasure & polymorphism
 */
class IArchetype
{
protected:
    World &m_world;
    Signature m_signature = 0;                                  //!< Signature of all the components stored in the `Archetype`
    std::unordered_map<Entity, size_t> m_entityToIndex;         //!< Maps an `Entity` to its index in the archetype columns
    std::unordered_map<ComponentID, size_t> m_componentToIndex; //!< Maps a `ComponentID` to its index in the archetype table

    virtual size_t requestTransfer(Entity, IArchetype *) = 0;
    virtual void transferComponentUnsafe(ComponentID, size_t, void *) = 0;
    virtual size_t allocateEntity(Entity) = 0;

    virtual void *getComponentUnsafe(ComponentID, Entity) = 0;

    /**
     * @brief Removes an `Entity` from this `Archetype`. All its components must've been removed already
     * 
     * @param entity 
     */
    void removeEntity(Entity entity);

public:
    friend World;

    virtual bool match(const Signature &sig) = 0;
    virtual bool hasEntity(Entity entity) = 0;

    /**
     * @brief Get a component for an Entity. Returns `nullptr` if the entity does not have this component
     *
     * @tparam T Type of the component
     * @param entity Entity from which to get the component
     * @return T*
     */
    template <typename T>
    T *get(Entity entity);

    /**
     * @brief Get the Signature of the `Archetype`
     *
     * @return Signature
     */
    Signature getSignature();
};

/**
 * @brief Stores components & entities matching the `Archetype`'s `Signature`, aka.
 *        entities whose components types are exactly the pack parameter of this `Archetype`
 *
 * @attention Once an archetype is created, it is only destroyed when its `World` is destroyed. However, an `Archetype` can be empty, and is quite lightweight in terms of memory
 *
 * @tparam Components
 */
template <typename... Components>
class Archetype : public IArchetype
{
public:
    using Row = std::tuple<Components...>;

private:
    std::tuple<std::vector<Components>...> m_table; //!< The actual table of components

    /**
     * @brief Gets the vector containing all the instances of a given `Component` in this `Archetype`.
     *
     * @tparam Component
     * @return std::vector<Component>&
     */
    template <typename Component>
    std::vector<Component> &getColumn();

    /**
     * @brief Transfers a component to this `Archetype` for a given `Entity`
     *
     * @warning Space must have been allocated for the entity first, using `Archetype::receiveEntity()`.
     *
     * @param component
     * @param entity
     * @param data
     */
    void transferComponentUnsafe(ComponentID component, size_t index, void *data) override;

    /**
     * @brief Allocates space for a given `Entity`, and asks the `from` `Archetype` to transfer its components next
     *
     * @param entity
     * @param from
     * @return size_t The row index where the entity is placed
     */
    size_t allocateEntity(Entity entity) override;

    /**
     * @brief Asks this `Archetype` to transfer all the components of a given entity
     *
     * @param to The `Archetype` that the entity is transferred to
     */
    size_t requestTransfer(Entity entity, IArchetype *to) override;

    /**
     * @brief Get a component pointer cast as a void pointer, from a component ID
     *
     * @param component
     * @param entity
     * @return void*
     */
    void *getComponentUnsafe(ComponentID component, Entity entity) override;

    Archetype(World &world);
    ~Archetype() = default;

public:
    friend class Commands;
    friend World;

    /** Declare self with other template specializations as friend */
    template <typename... Cs>
    friend class Archetype;

    /**
     * @brief Sets the component for an entity
     *
     * Overwrites the previous instance of the component
     *
     * @note Does nothing if the `Entity` or the `Component` type aren't stored in this archetype.
     *
     * @tparam Component
     * @param entity
     * @param component
     */
    template <typename Component>
    void set(Entity entity, Component component);

    /**
     * @brief Does this `Archetype` contains the components for the provided entity ?
     *
     * @param entity
     * @return true
     * @return false
     */
    bool hasEntity(Entity entity) override;

    /**
     * @brief Does this `Archetype`'s components match the given `Signature` ?
     *
     * @param sig
     * @return true
     * @return false
     */
    bool match(const Signature &sig) override;
};