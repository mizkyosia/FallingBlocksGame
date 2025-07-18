#pragma once

#include <_Entity.hpp>
#include <managers/SystemManager.hpp>

template <typename T>
T &Entity::getComponent() const
{
    // If the entity does not have this component, throw an error
    if (!s_Signatures[m_id][T::ComponentID()])
        throw std::runtime_error{"Entity N°" + std::to_string(m_id) + "has no component " + std::string{typeid(T).name()}};
    return ComponentManager::GetComponent<T>(m_id);
}

template <typename T>
T &Entity::addComponent(T component) const
{
    // Change the signature for the given component
    s_Signatures[m_id].set(T::ComponentID());
    SystemManager::EntitySignatureChanged(*this);
    return ComponentManager::AddComponent<T>(m_id, component);
}

template <typename T>
void Entity::removeComponent() const
{
    // Change the signature for the given component
    s_Signatures[m_id].reset(T::ComponentID());
    SystemManager::EntitySignatureChanged(*this);
    ComponentManager::RemoveComponent<T>(m_id);
}