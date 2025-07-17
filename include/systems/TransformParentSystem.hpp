#pragma once

#include <Components.hpp>
#include <systems/System.hpp>

namespace Systems
{
    /** \brief Updates child transform positions based on their parents */
    struct TransformParentSystem : public System<Components::Transform>
    {
        // Must be defined, called every frame
        void update(float deltaTime, const Entity::Set &entities) override
        {
            for (auto entity : entities)
            {
                if (auto parent = entity.getParent())
                {
                    auto &transform = entity.getComponent<Components::Transform>();
                    auto &parentTransform = parent->getComponent<Components::Transform>();

                    transform.localPosition = parentTransform.position - transform.position;
                    transform.localAngle = parentTransform.angle - transform.localAngle;
                    transform.localScale = sf::Vector2f{transform.scale.x / parentTransform.scale.x, transform.scale.y / parentTransform.scale.y};
                }
            }
        };
    };

} // namespace Systems
