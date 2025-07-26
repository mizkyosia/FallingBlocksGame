#pragma once
#include <cstdint>
#include <unordered_map>
#include <vector>
#include <filesystem>
#include <functional>
#include <future>

#include "../Global.hpp"
#include "Asset.hpp"

#ifndef NDEBUG
#include <iostream>
#include <boost/core/demangle.hpp>
#endif

using AssetType = std::uint8_t;
using Path = std::filesystem::path;

class AssetManager
{
public:
    static volatile constexpr AssetType MaxAssetTypes = 8;

private:
    AssetManager() = delete;
    ~AssetManager() = delete;

    struct IAssetCollection
    {
        virtual ~IAssetCollection() = default;
    };

    template <typename T>
    struct AssetCollection : public IAssetCollection
    {
        std::unordered_map<Path, Asset<T>> collection;

        template<typename ...Args>
        [[nodiscard]] Asset<T> getAsset(AssetManager& manager, Path path, Args... args);
    };

    std::vector<std::shared_ptr<IAssetCollection>> m_assetCollections; //!< Holds all the different asset collections, type-erased
    std::vector<std::future<void>> s_AssetLoaders;                     //!< Holds the threads currently loading assets
    AssetType m_nextAssetType;                                         //!< Next `AssetType` ID that will be given to the next registered asset type

    /** @brief Pools the asset loader threads, and remove them if they're completed */
    void poolLoadingAssets();

    /** @brief Gets the asset collection holding the given asset type */
    template <typename T>
    AssetCollection<T> &getAssetCollection();

    /**
     * @brief Get the `AssetType` ID of the given asset type
     *
     * @tparam T
     * @return AssetType
     */
    template <typename T>
    AssetType getAssetTypeID();

public:
    friend World;

    /**
     * @brief Registers a new type of loadable asset data
     */
    template <typename Data>
    AssetType registerAsset();

    /**
     * @brief Gets the `Asset` of the specified type, loaded from the specified path
     *
     * Creates the `Asset`  if it wasn't loaded yet
     *
     * @tparam T Type of the `Asset`
     * @tparam Args Arguments forwarded to the builder of the `Asset` (specified when registering the `Asset`)
     * @param path The path from which to load the `Asset`
     * @return T
     */
    template <typename T>
    Asset<T> get(Path path);
};