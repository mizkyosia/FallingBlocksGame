#pragma once
#include <SFML/Graphics.hpp>
#include <managers/AssetManager.hpp>

#include <boost/core/demangle.hpp>

class App
{
private:
    /**
     * The SFML Window & Renderer
     */
    sf::RenderWindow m_window;

    void loop();

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

    /** \brief Register the given structs/classes as asset types */
    template <typename... _AssetTypes>
    void registerAssetTypes()
    {
        (_registerAssetType<_AssetTypes>(), ...);
    }
    sf::RenderWindow &window() { return m_window; };

    void run();
};
