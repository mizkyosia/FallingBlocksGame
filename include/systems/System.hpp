#pragma once
#include <unordered_set>
#include <thread>

#include <Entity.hpp>

// Forward declaration for private member acces
class SystemManager;

namespace Systems
{
    /** \brief Virtual interface for storing Systems */
    struct BaseSystem
    {
    private:
        Entity::Set m_entities;
        Entity::Signature m_signature;

    public:
        friend class ::SystemManager;
        template <typename... _Components>
        friend class System;

        /**
         * \brief Create a new base system
         */
        BaseSystem() : m_entities{}, m_signature(0) {};
        virtual ~BaseSystem() = default;

        virtual void update(float deltaTime, const Entity::Set &entities) = 0;

        const Entity::Set &entities() const { return m_entities; }
        const Entity::Signature &signature() const { return m_signature; }
    };

    /**
     * \brief Virtual class representing an ECS System
     * \tparam _Components List of the components an entity must have for this system to act upon it
     */
    template <typename... _Components>
    struct System : public BaseSystem
    {
    private:
        template <typename T>
        void _addToSignature()
        {
            m_signature.set(T::ComponentID());
        }

    protected:
        /**
         * \brief Constructs a new system and initializes it
         */
        System()
        {
            // Fold expression to call all components, and add their ID to the signature
            (_addToSignature<_Components>(), ...);
        };
        virtual ~System() = default;
    };

} // namespace Systems
