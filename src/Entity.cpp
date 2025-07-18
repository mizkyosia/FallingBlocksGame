#include <Entity.hpp>
#include <managers/SystemManager.hpp>

void Entity::InitIDs()
{
    for (ID i = 0; i < MaxEntities; i++)
    {
        s_ValidEntityIDs.push(i);
    }
}

Entity::Entity() : m_id(s_ValidEntityIDs.back())
{
    // Remove the entity ID we just gave this entity
    s_ValidEntityIDs.pop();
    // Add the entity into the global set
    Entity &ref = *this;
    s_LivingEntities.insert(ref);
}

Entity::Entity(const Entity &other) : m_id(other.m_id) {}

void Entity::destroy()
{
    // Destroy all of its components
    ComponentManager::EntityDestroyed(m_id);
    
    // Remove it from all systems
    SystemManager::EntityDestroyed(*this);

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
}

std::optional<Entity> Entity::getParent() const
{
    if (s_Parents.contains(m_id))
        return s_Parents[m_id];
    else
        return std::nullopt;
}

Entity::Signature Entity::getSignature() const
{
    return s_Signatures[m_id];
}

Entity::ID Entity::id() const { return m_id; }

bool Entity::operator==(const Entity &other) const { return other.m_id == m_id; }
