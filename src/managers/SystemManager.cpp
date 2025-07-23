#include <managers/SystemManager.hpp>

void SystemManager::EntityDestroyed(Entity entity)
{
    // Removes the entity from all systems
    for (auto &system : s_Systems)
        system->m_entities.erase(entity);
}

void SystemManager::EntitySignatureChanged(Entity entity)
{
    Signature entitySig = entity.getSignature();
    for (auto &system : s_Systems)
    {
        auto systemSig = system->signature();
        // If the entity has all necessary components
        if ((systemSig & entitySig) == systemSig)
            system->m_entities.insert(entity);
        else
            system->m_entities.erase(entity);
    }
}

void SystemManager::Update(float deltaTime, App& app)
{
    for (auto &system : s_Systems)
        system->update(deltaTime, app);
}