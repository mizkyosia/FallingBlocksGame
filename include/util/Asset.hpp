#pragma once
#include <bits/shared_ptr.h>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

class AssetsManager;

/**
 * Represents a handle to an asset that may or may not have been loaded yet.
 *
 * A worse version of a `shared_ptr`, but easier to manage
 * \tparam _AssetType Type of the underlying asset, e.g `sf::Texture`
 */
template <typename _AssetType>
class Asset
{
private:
    friend class AssetsManager;

    /**
     * \brief Actual pointer to the asset. Using a `shared_ptr` so it is correctly freed
     */
    std::shared_ptr<_AssetType> m_handle;

    /**
     * \brief Is the asset loaded yet ? If not, the value inside the handle may be wrong.
     */
    bool *m_loaded;

public:
    /// @brief Creates a new empty asset
    Asset() : m_handle(std::make_shared<_AssetType>())
    {
        bool _ = false;
        m_loaded = &_;
    };

    /**
     * \brief Create an asset abstraction from a given pointer
     * \param handle The shared pointer containing the actual asset
     */
    Asset(std::shared_ptr<_AssetType> handle) : m_handle(handle)
    {
        bool _ = false;
        m_loaded = &_;
    };

    /**
     * \brief Copies the necessary data from another asset handle
     * \param other
     */
    Asset(const Asset &other) : m_handle(other.m_handle), m_loaded(other.m_loaded) {};

    /**
     * \brief Dereferencing to give access to the underlying asset
     */
    _AssetType &operator*() { return *m_handle; }

    /**
     * \brief Arrow access operator
     */
    _AssetType *operator->() { return m_handle; }

    /**
     * \brief Assignment operator
     */
    Asset<_AssetType> &operator=(const Asset<_AssetType> &other)
    {
        m_handle = other.m_handle;
        m_loaded = other.m_loaded;
        return *this;
    }

    /**
     * \brief Is the asset ready ?
     */
    bool loaded() const { return *m_loaded; }
};

using TextureAsset = Asset<sf::Texture>;
using FontAsset = Asset<sf::Font>;
using SoundAsset = Asset<sf::SoundBuffer>;
using ShaderAsset = Asset<sf::Shader>;