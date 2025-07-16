#pragma once
#include <SFML/Graphics.hpp>
#include <managers/ComponentManager.hpp>
#include <managers/AssetManager.hpp>

class App
{
private:
    /**
     * The SFML Window & Renderer
     */
    sf::RenderWindow m_window;

    void loop();

public:
    App();
    ~App();

    /** \brief Register the given struct/class type as a component type */
    template<typename T>
    void registerComponent() {
        T::s_ComponentID = ComponentManager::Register<T>();
    }

    /** \brief Register the given struct/class type as an asset type */
    template<typename T>
    void registerAssetType() {
        T::s_AssetTypeID = AssetManager::Register<T>();
    }

    void run();
};
