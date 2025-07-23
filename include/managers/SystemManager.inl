#pragma once
#include <managers/SystemManager.hpp>

template <typename... _Components>
inline SystemManager::System<_Components...>::System(SystemFunction<_Components...> innerSystem) : system(innerSystem), query()
{
}

template <typename... _Components>
inline void SystemManager::System<_Components...>::run(float deltaTime, App &app)
{
    system(deltaTime, app, query.entities);
}

template <typename... _Components>
inline SystemManager::SystemWrapper::SystemWrapper(SystemFunction<_Components...> innerSystem) : system(std::make_unique<System<_Components...>>(innerSystem))
{
}