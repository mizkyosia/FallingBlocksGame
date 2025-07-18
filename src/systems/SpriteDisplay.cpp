#include <systems/SpriteDisplay.hpp>
#include <App.hpp>

void Systems::SpriteDisplay::update(float deltaTime, App &app)
{
    for (auto entity : entities())
    {
        auto &sprite = entity.getComponent<Components::Sprite>();

        if (!sprite.texture.loaded())
            continue;

        // Means the sprite is not displayed
        if (sprite.sprite.getTextureRect().size == sf::Vector2i{0, 0})
        {
            sprite.sprite.setTextureRect(sf::IntRect(sf::Vector2i{0, 0}, sf::Vector2i(sprite.texture->getSize())));
        }

        auto &transform = entity.getComponent<Components::Transform>();

        sprite.sprite.setPosition(transform.position);
        sprite.sprite.setRotation(sf::radians(transform.angle));
        sprite.sprite.setScale(transform.localScale);

        app.window().draw(sprite.sprite);
    }
};