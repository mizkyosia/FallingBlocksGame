#include <managers/ComponentManager.hpp>
#include <algorithm>

void ComponentManager::EntityDestroyed(EntityID entity)
{
    for (auto &map : s_ComponentMaps)
    {
        map->deleteComponent(entity);
    }
}

void ComponentManager::AddComponent(EntityID entity, ComponentID componentType)
{
    // Dirty trick they don't want you to know about (virtual functions)
    s_ComponentMaps[componentType]->insertComponent(entity);
}