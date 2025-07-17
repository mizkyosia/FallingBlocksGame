#pragma once
#include <SFML/Graphics.hpp>
#include <managers/ComponentManager.hpp>
#include <managers/AssetManager.hpp>
#include <managers/SystemManager.hpp>

#include <boost/core/demangle.hpp>

class App
{
private:
    /**
     * The SFML Window & Renderer
     */
    sf::RenderWindow m_window;

    void loop();

    template <typename _Component>
    void _registerComponent()
    {
        if (_Component::s_ComponentID != ComponentManager::MaxComponents)
            throw std::runtime_error("Error : trying to register component " + boost::core::demangle(typeid(_Component).name()) + " more than once !");
        _Component::s_ComponentID = ComponentManager::Register<_Component>();
    }

    template <typename _AssetType>
    void _registerAssetType()
    {
        if (_AssetType::s_AssetTypeID != AssetManager::MaxAssetTypes)
            throw std::runtime_error("Error : trying to register asset type " + boost::core::demangle(typeid(_AssetType).name()) + " more than once !");
        _AssetType::s_AssetTypeID = AssetManager::Register<_AssetType>();
    }

public:
    App();
    ~App();

    /** \brief Register the given structs/classes as component types */
    template <typename... _Components>
    void registerComponents()
    {
        // Fold expression
        (_registerComponent<_Components>(), ...);
    }

    /** \brief Register the given structs/classes as asset types */
    template <typename... _AssetTypes>
    void registerAssetTypes()
    {
        (_registerAssetType<_AssetTypes>(), ...);
    }

    /** \brief Register the given structs/classes as ECS Systems */
    template <typename... _Systems>
    void registerSystems()
    {
        (SystemManager::Register<_Systems>(), ...);
    }

    void run();
};
