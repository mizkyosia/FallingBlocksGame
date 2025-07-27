#pragma once
#include <bits/shared_ptr.h>
#include <functional>
#include <filesystem>
#include "Global.hpp"

class App;

/**
 * Represents a handle to an asset that may or may not have been loaded yet.
 *
 * A worse version of a `shared_ptr`, but easier to manage
 * @tparam _AssetType Type of the underlying data, e.g `sf::Texture`
 * @tparam _AssetType Type of the class derived from the asset, e.g `Core::Texture`
 */
template <typename DataType>
struct Asset
{
    using SelfType = Asset<DataType>;
    using Path = std::filesystem::path;

private:
    /**
     * @brief Actual pointer to the asset. Using a `shared_ptr` so it is correctly freed
     */
    std::shared_ptr<DataType> m_handle;

    /**
     * @brief Is the asset loaded yet ? If not, the value inside the handle may be wrong.
     */
    bool *m_loaded;

protected:
    /** @brief Event function, called once when the asset is loaded */
    void whenLoaded() {};

    Asset(DataType data) : m_handle(std::make_shared(data)), m_loaded(new bool(false)) {};

public:
    friend World;
    friend class AssetManager;

    Asset() : m_handle(std::make_shared<DataType>()), m_loaded(new bool(false)) {};

    /**
     * @brief Copies the necessary data from another asset handle of the same type
     * \param other
     */
    Asset(const SelfType &other) : m_handle(other.m_handle), m_loaded(other.m_loaded) {};

    /**
     * @brief Is the asset ready ?
     */
    bool loaded() const { return *m_loaded; }

    /**
     * @brief Returns the unerlying pointer to the asset data
     */
    std::shared_ptr<DataType> handle() { return m_handle; };

    /** @brief Equality operator */
    bool operator==(const SelfType &other)
    {
        return m_handle == other.m_handle;
    }

    /** @brief Reference access operator for the data linked to the asset handle */
    DataType &operator*()
    {
        return *m_handle;
    }

    /** @brief Reference access operator for the data linked to the asset handle */
    const DataType &operator*() const
    {
        return *m_handle;
    }

    /** @brief Reference access operator for the data linked to the asset handle */
    DataType *operator->()
    {
        return m_handle.get();
    }

    /** @brief Reference access operator for the data linked to the asset handle */
    const DataType *operator->() const
    {
        return m_handle.get();
    }
};