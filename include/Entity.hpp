#pragma once
#include <bitset>
#include <cstdint>
#include <queue>
#include <unordered_set>
#include <concepts>
#include <optional>
#include <components/Component.hpp>

class App;

struct Entity
{
public:
    using Signature = std::bitset<ComponentManager::MaxComponents>;
    using ID = EntityID;

    /** \brief Custom hash functor, required for hashing containers */
    struct Hasher
    {
        size_t operator()(const Entity &entity) const
        {
            return std::hash<EntityID>()(entity.m_id);
        };
    };

    using Set = std::unordered_set<Entity, Hasher>;

private:
    friend Hasher;

    /** \brief Unique ID of this entity */
    ID m_id;

    /** \brief Remaining valid Entity IDs that will be attributed to newly created entities. Destroyed entities automatically refill this queue with their ID */
    inline static std::queue<ID> s_ValidEntityIDs;

    /** \brief All living entities in the game world */
    inline static Set s_LivingEntities;

    /** \brief Maps an entity to its children, by ID */
    inline static std::unordered_map<ID, Set> s_Children;

    /** \brief Maps an entity to its parent entity, if it has one */
    inline static std::unordered_map<ID, Entity> s_Parents;

    /** \brief Maps an entity to its signature */
    inline static std::unordered_map<ID, Signature> s_Signatures;

public:
    /**
     * \brief Initializes the available IDs by pushing them all onto the queue
     */
    static void InitIDs();

    static constexpr ID MaxEntities = 5000;

    /** \brief Creates a new entity in memory */
    Entity();
    /** \brief Copy constructor */
    Entity(const Entity &other);
    ~Entity();

    /** \brief Gets the parent of the entity if it has one */
    std::optional<Entity> getParent() const;

    /** \brief Gets the signature of the entity */
    Signature getSignature() const;

    /** \brief Gets the component of type `T` attached to this entity */
    template <typename T>
    [[nodiscard]] T &getComponent() const;

    /** \brief Adds a component of type `T` to this entity */
    template <typename T>
    T &addComponent(T component) const;

    /** \brief Removes the component of type `T` attached to this entity, if it exists */
    template <typename T>
    void removeComponent() const;

    /** \brief Returns the internal ID of the entity */
    [[nodiscard]] EntityID id() const;

    /** \brief Verifies the equality between 2 entities, by checking their internal IDs */
    bool operator==(const Entity &other) const;
};