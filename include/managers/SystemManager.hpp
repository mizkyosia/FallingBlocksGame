#pragma once
#include <vector>
#include <bits/unique_ptr.h>
#include <systems/System.hpp>
#include <thread>

template <typename... _Components>
using SystemFunction = std::function<float, App &, const Entity::Query<_Components...> &>;

class SystemManager
{
private:
    /** \brief Contains a reference to all the systems currently running */
    inline static std::vector<std::shared_ptr<Systems::BaseSystem>> s_Systems;

    template <typename T>
    static void Register()
    {
        std::shared_ptr<T> system = std::make_shared<T>();
        s_Systems.push_back(std::static_pointer_cast<Systems::BaseSystem>(system));
    };

    struct ISystem
    {
        virtual void run(float deltaTime, App &app);
        virtual ~ISystem() = default;
    };

    template <typename... _Components>
    struct System : public ISystem
    {
        Entity::Query<_Components...> query;
        SystemFunction<_Components...> system;

        /** \brief Builds a system from the following system function */
        System(SystemFunction<_Components...> innerSystem);

        void run(float deltaTime, App &app) override;
    };

    /** \brief Thin wrapper struct around a system. Used only for implicit conversion & to remove type parameters from declaration */
    struct SystemWrapper
    {
        /** \brief Pointer to a system. Using the base virtual class to remove type parameters */
        std::unique_ptr<ISystem> system;

        /** \brief Converting constructor for implicit conversion */
        template <typename... _Components>
        SystemWrapper(SystemFunction<_Components...> innerSystem);
    };

public:
    friend class App;

    // Static class, no instantiation
    SystemManager() = delete;
    ~SystemManager() = delete;

    /** \brief Called when an entity's signature has changed. Updates all systems accordingly */
    static void EntitySignatureChanged(Entity entity);

    /** \brief Called when an entity is destroyed. Updates all systems accordingly */
    static void EntityDestroyed(Entity entity);

    /** \brief Updates all of the systems */
    static void Update(float deltaTime, App &app);
};

#include <managers/SystemManager.inl>
