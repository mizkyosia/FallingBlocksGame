#pragma once
#include <bitset>
#include <cstdint>
#include <unordered_map>
#include <bits/shared_ptr.h>
#include <stdexcept>

// Forward declaration
class Entity{
    public: using ID = uint32_t;
};

namespace Components
{
    /**
     * \brief Virtual class, base of all components
     *
     * Used for abstraction & component management purposes
     */
    class Component
    {
    public:
        /**
         * \brief Type ID of a component type
         */
        using ComponentType = std::uint8_t;

        /**
         * \brief Maximum number of different types of components that can be registered
         */
        static constexpr ComponentType MaxComponents = 32;

    private:
        class IComponentMap
        {
        public:
            virtual ~IComponentMap() = default;
            virtual void deleteComponent(Entity::ID entity) = 0;
        };

        /**
         * \brief Retains information about all the instances of a specific component, and the entities they are attached to
         */
        template <typename T>
        class ComponentMap : public IComponentMap
        {
        public:
            std::unordered_map<Entity::ID, T> m_map;

            /**
             * \brief Inserts a component instance mapped from the entity it is attached to
             */
            void insertComponent(Entity::ID entity, T component)
            {
                m_map.insert(entity, component);
            };
            /**
             * \brief Gets the component associated to the entity
             */
            T &getComponent(Entity::ID entity)
            {
                return m_map[entity];
            };
            /**
             * \brief Deletes the component associated to the entity
             */
            void deleteComponent(Entity::ID entity) override
            {
                m_map.erase(entity);
            };
        };

        /**
         * \brief Fetches the component map associated to the given component type, and returns it
         */
        template <typename T>
        static std::shared_ptr<ComponentMap<T>> GetComponentMap()
        {
            auto name = typeid(T).name();
            if (!s_ComponentMaps.contains(name))
                throw std::runtime_error(std::string{"Unknown component type : "} + name);
            return std::static_pointer_cast<ComponentMap<T>>(s_ComponentMaps[name]);
        }

        /**
         * \brief Associates type name to ComponentMap
         */
        static std::unordered_map<const char *, std::shared_ptr<IComponentMap>> s_ComponentMaps;
        /**
         * \brief Associates type name to ComponentType ID
         */
        static std::unordered_map<const char *, ComponentType> s_ComponentTypes;

        /**
         * \brief The next registered component will be given this type ID
         */
        static ComponentType NextComponentTypeID;

    public:
        /**
         * \brief Constructs a component and registers its type if needed
         * \tparam T The type of the class derived from `Component`
         */
        template <class T>
        Component()
        {
            const char *type = typeid(T).name();
            auto t = s_ComponentTypes.find(type);
            // Tries to register the component if it isn't already
            if (t == s_ComponentTypes.end())
            {
                // Register component type
                s_ComponentTypes.insert({type, NextComponentTypeID});
                // And create a new map
                s_ComponentMaps.insert({type, std::make_shared<ComponentMap<T>>()});
                NextComponentTypeID++;
            }
        };
        virtual ~Component() = default;

        /**
         * \brief Adds a new instance of the component, associated to a given entity
         * \tparam T Type of the component
         */
        template <typename T>
        static void AddComponent(Entity::ID entity, T component)
        {
            GetComponentMap<T>()->insertComponent(entity);
        };

        /**
         * \brief Deletes the component instance associated to the given entity
         * \tparam T Type of the component
         */
        template <typename T>
        static void RemoveComponent(Entity::ID entity)
        {
            GetComponentMap<T>()->deleteComponent(entity);
        };

        /**
         * \brief Gets the component instance associated to the given entity
         * \tparam T Type of the component
         */
        template <typename T>
        static T &GetComponent(Entity::ID entity)
        {
            return GetComponentMap<T>()->getComponent(entity);
        };

        /**
         * \brief Gets the component type id associated to the component
         * \tparam T Type of the component
         */
        template <typename T>
        static ComponentType GetComponentType()
        {
            auto name = typeid(T).name();
            if (!s_ComponentTypes.contains(name))
                throw std::runtime_error(std::string{"Unknown component type : "} + name);
            return s_ComponentTypes[name];
        };
    };

} // namespace Components