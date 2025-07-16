#pragma once
#include <bitset>
#include <cstdint>
#include <queue>
#include <unordered_set>
#include <concepts>
#include <components/Component.hpp>

class App;

class Entity
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
    static void InitIDs()
    {
        for (ID i = 0; i < MaxEntities; i++)
        {
            s_ValidEntityIDs.push(i);
        }
    };

    static constexpr ID MaxEntities = 5000;

    Entity() : m_id(s_ValidEntityIDs.back())
    {
        // Remove the entity ID we just gave this entity
        s_ValidEntityIDs.pop();
        // Add the entity into the global set
        Entity &ref = *this;
        s_LivingEntities.insert(ref);
    };
    ~Entity()
    {
        // Destroy all of its components
        ComponentManager::EntityDestroyed(m_id);
        // Make the entity ID available again
        s_ValidEntityIDs.push(m_id);
        // Remove this entity from the global list
        s_LivingEntities.erase(*this);

        auto parent = s_Parents.find(m_id);
        // If it has a parent, append its children to its parent
        if (parent != s_Parents.end())
            for (auto &child : s_Children[m_id])
                s_Parents[child.m_id] = parent->second;
        // Otherwise, make them parentless
        else
            for (auto child : s_Children[m_id])
                s_Parents.erase(child.m_id);
    };

    /** \brief Gets the component of type `T` attached to this entity */
    template <typename T>
    [[nodiscard]] T &getComponent()
    {
        // If the entity does not have this component, throw an error
        if (!s_Signatures[m_id][T::ComponentID()])
            throw std::runtime_error{"Entity N°" + std::to_string(m_id) + "has no component " + std::string{typeid(T).name()}};
        return ComponentManager::GetComponent<T>(m_id);
    }

    /** \brief Adds a component of type `T` to this entity */
    template <typename T>
    T &addComponent(T component)
    {
        // Change the signature for the given component
        s_Signatures[m_id].set(T::ComponentID());
        return ComponentManager::AddComponent<T>(m_id, component);
    }

    /** \brief Removes the component of type `T` attached to this entity, if it exists */
    template <typename T>
    void removeComponent()
    {
        // Change the signature for the given component
        s_Signatures[m_id].reset(T::ComponentID());
        ComponentManager::RemoveComponent<T>(m_id);
    }

    /** \brief Returns the internal ID of the entity */
    [[nodiscard]] EntityID id() const { return m_id; }

    /** \brief Verifies the equality between 2 entities, by checking their internal IDs */
    bool operator==(const Entity &other) const { return other.m_id == m_id; }
};