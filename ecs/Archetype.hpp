#pragma once
#include <vector>
#include <unordered_map>
#include <type_traits>
#include <bits/unique_ptr.h>

#include "Global.hpp"

/**
 * @brief Base virtual class of all `Archetype`s. Used solely for type erasure & polymorphism
 */
class IArchetype
{
public:
    virtual bool match(const Signature &sig) = 0;
    virtual bool hasEntity(Entity entity) = 0;
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
    /** Assures that the given type is part of this `Archetype`'s `Components` */
    template <typename C>
    concept ArchetypeComponent = InPack<C, Components...>;

    using Row = std::tuple<Components...>;

private:
    Signature m_signature;                              /** Signature of all the components stored in the `Archetype` */
    std::unordered_map<Entity, size_t> m_entityToIndex; /** Maps an `Entity` to its index in the archetype columns */
    std::tuple<std::vector<Components>...> m_table;     /** The actual table of components */

    /**
     * @brief Insert an entity into this `Archetype`, along with all its components
     *
     * @warning The entity must have components matching this `Archetype`
     *
     * @param entity The entity to insert
     * @param components Its components
     */
    void insertEntity(Entity entity, Components... components);

    /**
     * @brief Removes an entity from this `Archetype`,an returns all of its components
     *
     * @param entity
     */
    Row removeEntity(Entity entity);

    /**
     * @brief Transfers a component **to** this `Archetype ` **from another**.
     *
     * In the case this component is not part of this `Archetype`'s components, it will simply be ignored.
     *
     * @tparam Component
     */
    template <typename Component>
    void transferComponent(Component comp);

    /**
     * @brief Moves an `Entity` and its components **from** this `Archetype` **to** another
     *
     * @tparam NewComponents
     */
    template <typename... NewComponents>
    void moveEntity(Archetype<NewComponents...> &newArchetype);

    /**
     * @brief Gets the vector containing all the instances of a given `Component` in this `Archetype`.
     *
     * @tparam Component
     * @return std::vector<Component>&
     */
    template <typename Component>
    std::vector<Component> &getColumn();

    Archetype();
    ~Archetype() = default;

public:
    friend class Commands;
    friend World;

    /** Declare self with other template specializations as friend */
    template <typename... Cs>
    friend Archetype<Cs...>;

    /**
     * @brief Gets a component attached to an entity. Returns `nullptr` if the entity is not in this `Archetype`
     *
     * @tparam Component
     * @param entity
     */
    template <ArchetypeComponent Component>
    Component *get(Entity entity);

    /**
     * @brief Sets the component for an entity. Overwrites the previous instance of this component
     *
     * @tparam Component
     * @param entity
     * @param component
     */
    template <ArchetypeComponent Component>
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

using ArchetypePtr = std::unique_ptr<IArchetype>;