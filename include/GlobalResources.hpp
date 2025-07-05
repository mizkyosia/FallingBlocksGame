#pragma once
#include <SFML/Graphics.hpp>

/**
 * Global resources needed to be available from anywhere in the app
 * Implemented as a thread-safe & non-instantiable singleton
 */
class GlobalResources
{
private:
    /**
     * Unique constructor
     * \warning Do not create a new instance of this class. Call `GlobalResources::getInstance()` instead
     */
    GlobalResources() {}

public:
    static GlobalResources &getInstance()
    {
        static GlobalResources instance; // Guaranteed to be destroyed.
                                         // Instantiated on first use.
        return instance;
    }

    GlobalResources(GlobalResources const &) = delete;
    void operator=(GlobalResources const &) = delete;

    /// Global font used by the UI
    sf::Font uiFont{"assets/fonts/VCR_OSD_MONO.ttf"};

    /// Shader used by all buttons, to give the hover & click effect
    sf::Shader buttonShader{std::filesystem::path("assets/shaders/button.frag"), sf::Shader::Type::Fragment};

    /**
     * Current mouse state, updated every frame
     */
    struct MouseState
    {
        /// Has a (left) mouse click been issued this frame ?
        bool clicked{false};
        /// Is the left button of the mouse currently down ?
        bool down{false};
        /// Current mouse cursor position, in world coordinates
        sf::Vector2f currentPosition{0.0, 0.0};
        /// The position of the mouse at the moment it was last pressed down
        sf::Vector2f pressedPosition{-100.0, -100.0};
    } mouseState;
};
