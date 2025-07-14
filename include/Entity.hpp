#pragma once
#include <bitset>
#include <cstdint>
#include <queue>
#include <components/Component.hpp>


class App;

class Entity
{
public:
    using Signature = std::bitset<Components::Component::MaxComponents>;
    using ID = std::uint32_t;

private:
    ID m_id;

    static std::queue<ID> s_ValidEntityIDs;

    static uint32_t s_LivingEntitiesCount;

    /**
     * \brief Initializes the available IDs by pushing them all onto the queue
     */
    static void InitIDs()
    {
        for (ID i = 0; i < MaxEntities; i++)
        {
            s_ValidEntityIDs.push(i);
        }
    };

public:
    static const ID MaxEntities = 5000;

    Entity() : m_id(s_ValidEntityIDs.back())
    {
        // Remove the entity ID we just gave this entity
        s_ValidEntityIDs.pop();
    };
    ~Entity()
    {
        // Make the entity ID available again
        s_ValidEntityIDs.push(m_id);
        s_LivingEntitiesCount--;
    };
};