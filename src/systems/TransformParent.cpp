
#include <systems/TransformParent.hpp>
#include <App.hpp>

void Systems::TransformParent::update(float deltaTime, App &app)
{
    for (auto entity : entities())
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