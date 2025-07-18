#pragma once
#include <vector>
#include <bits/unique_ptr.h>
#include <systems/System.hpp>
#include <thread>

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
    static void Update(float deltaTime, App& app);
};