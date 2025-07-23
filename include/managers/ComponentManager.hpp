#pragma once
#include <cstdint>
#include <bits/shared_ptr.h>
#include <unordered_map>
#include <vector>
#include <stdexcept>

#include <Global.hpp>

#ifndef NDEBUG
#include <iostream>
#include <boost/core/demangle.hpp>
#endif

class ComponentManager
{
public:
    /** Declare App as friend for access */
    friend class App;

    /**
     * \brief Maximum number of different types of components that can be registered
     */
    static constexpr ComponentID MaxComponents = 32;

private:
    /**
     * \brief Pure virtual class, used for serializing, deserializing & acessing components without worrying about types
     */
    class IComponentMap
    {
    public:
        virtual ~IComponentMap() = default;
        virtual void deleteComponent(Entity entity) = 0;
        virtual void insertComponent(Entity entity) = 0;
    };

    /**
     * \brief Retains information about all the instances of a specific component, and the entities they are attached to
     */
    template <typename T>
    class ComponentMap : public IComponentMap
    {
    public:
        std::unordered_map<Entity, T> m_map;

        /** \brief Attaches a component to a given entity */
        T &insertComponent(Entity entity, T component);

        /** \brief Creates & inserts a default component, and attaches it to the entity */
        void insertComponent(Entity entity) override;

        /** \brief Gets the component associated to the entity */
        [[nodiscard]] T &getComponent(Entity entity);

        /** \brief Deletes the component associated to the entity */
        void deleteComponent(Entity entity) override;
    };

    /**
     * \brief Fetches the component map associated to the given component type, and returns it
     * \tparam Type of the desired components in the map
     */
    template <typename T>
    static std::shared_ptr<ComponentMap<T>> GetComponentMap();

    template <typename T>
    static ComponentID Register();

    /** \brief Associates ComponentType ID to ComponentMap */
    inline static std::vector<std::shared_ptr<IComponentMap>> s_ComponentMaps;

public:
    /** \warning The component manager is a static class and should be treated as such. Do not instantiate it */
    ComponentManager() = delete;
    ComponentManager(ComponentManager &src) = delete;
    ~ComponentManager() = delete;

    /**
     * \brief Adds a new instance of the component, associated to a given entity
     * \tparam T Type of the component
     */
    template <typename T>
    static T &AddComponent(Entity entity, T component);

    /** \brief Instantiates a default component from the given ComponentType and attaches it to the given Entity */
    static void AddComponent(Entity entity, ComponentID componentType);

    /**
     * \brief Deletes the component instance associated to the given entity
     * \tparam T Type of the component
     */
    template <typename T>
    static void RemoveComponent(Entity entity);

    /**
     * \brief Gets the component instance associated to the given entity
     * \tparam T Type of the component
     */
    template <typename T>
    [[nodiscard]] static T &GetComponent(Entity entity);

    /** \brief Called when an entity is destroyed. Removes all of its components */
    static void EntityDestroyed(Entity entity);
};

template <typename T>
inline T &ComponentManager::ComponentMap<T>::insertComponent(Entity entity, T component)
{
    m_map.insert({entity, component});
    return m_map[entity];
}

template <typename T>
inline void ComponentManager::ComponentMap<T>::insertComponent(Entity entity)
{
    m_map.insert({entity, T()});
}

template <typename T>
inline T &ComponentManager::ComponentMap<T>::getComponent(Entity entity)
{
    return m_map[entity];
}

template <typename T>
inline void ComponentManager::ComponentMap<T>::deleteComponent(Entity entity)
{
    m_map.erase(entity);
}

template <typename T>
inline ComponentID ComponentManager::Register()
{
    // Create a new map
    auto map = std::make_shared<ComponentMap<T>>();
    // Fetch the next component ID (number of already registered components)
    ComponentID id = s_ComponentMaps.size();

#ifndef NDEBUG
    std::cout << "Registered component " << boost::core::demangle(typeid(T).name()) << " with ComponentType " << (int)id << std::endl;
#endif

    // Add the component map
    s_ComponentMaps.push_back(map);
    return id;
}

template <typename T>
inline T &ComponentManager::AddComponent(Entity entity, T component)
{
    return GetComponentMap<T>()->insertComponent(entity, component);
}

template <typename T>
inline void ComponentManager::RemoveComponent(Entity entity)
{
    GetComponentMap<T>()->deleteComponent(entity);
}

template <typename T>
inline T &ComponentManager::GetComponent(Entity entity)
{
    return GetComponentMap<T>()->getComponent(entity);
}

template <typename T>
inline std::shared_ptr<ComponentManager::ComponentMap<T>> ComponentManager::GetComponentMap()
{
    return std::static_pointer_cast<ComponentMap<T>>(s_ComponentMaps[T::ComponentID()]);
}