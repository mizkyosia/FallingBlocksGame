#include <managers/ComponentManager.hpp>
#include <algorithm>

void ComponentManager::EntityDestroyed(Entity entity)
{
    for (auto &map : s_ComponentMaps)
    {
        map->deleteComponent(entity);
    }
}

void ComponentManager::AddComponent(Entity entity, ComponentID componentType)
{
    // Dirty trick they don't want you to know about (virtual functions)
    s_ComponentMaps[componentType]->insertComponent(entity);
}