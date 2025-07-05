#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include <GlobalResources.hpp>

/**
 * A drawable button, extending the `sf::RectangleShape` class
 * Can have a texture and/or a shader optionally bound to it
 *
 * \warning The given shader must have the "clicked" and "hovered" boolean uniforms declared
 */
class Button : public sf::Drawable
{
protected:
    sf::RectangleShape m_shape;
    /**
     * An optional shader to apply to the button's texture
     */
    sf::Shader *m_shader;

public:
    Button(sf::Vector2f size = {0.0, 0.0}, sf::Texture *texture = nullptr, sf::Shader *shader = nullptr) : m_shape(size), m_shader(shader)
    {
        m_shape.setTexture(texture);
    }

    void draw(sf::RenderTarget &target, sf::RenderStates states) const override
    {
        // Set the shader values if needed
        if (m_shader != nullptr)
        {
            states.shader = m_shader;
            m_shader->setUniform("clicked", clicked());
            m_shader->setUniform("hovered", hovered());
        }
        // Draw the underlying shape using our renderstate
        target.draw(m_shape, states);
    }

    sf::Shader *getShader() const { return m_shader; };

    /**
     * Has the button been clicked this frame ?
     */
    bool clicked() const
    {
        GlobalResources &res = GlobalResources::getInstance();
        return m_shape.getGlobalBounds().contains(res.mouseState.currentPosition) && res.mouseState.clicked;
    };
    /**
     * Has the button been hovered this frame ?
     */
    bool hovered() const
    {
        GlobalResources &res = GlobalResources::getInstance();
        return m_shape.getGlobalBounds().contains(res.mouseState.currentPosition);
    };

    /**
     * Is the mouse down over the button ?
     * Does not mean the button has been clicked
     */
    bool mouseDown() const
    {
        GlobalResources &res = GlobalResources::getInstance();
        return m_shape.getGlobalBounds().contains(res.mouseState.currentPosition) && res.mouseState.down;
    }

    /**
     * Sets the shader used by the button
     * \warning must be a fragment shader
     */
    void setShader(sf::Shader *shader) { m_shader = shader; }

    sf::RectangleShape &shape() { return m_shape; };
};