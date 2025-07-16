#pragma once
#include <cstdint>
#include <unordered_map>
#include <vector>
#include <filesystem>
#include <functional>
#include <future>

#ifndef NDEBUG
#include <iostream>
#include <boost/core/demangle.hpp>
#endif

using AssetType = std::uint8_t;

class AssetManager
{
public:
    static volatile constexpr AssetType MaxAssetTypes = 8;

private:
    using Path = std::filesystem::path;
    AssetManager() = delete;
    ~AssetManager() = delete;

    friend class App;

    struct IAssetCollection
    {
    };

    template <typename T>
    struct AssetCollection : public IAssetCollection
    {
        std::unordered_map<Path, T> collection;

        [[nodiscard]] T getAsset(Path path);
        void insertAsset(Path path, T asset);
        void removeAsset(Path path);
    };

    /** \brief Holds all the different asset collections */
    inline static std::vector<std::shared_ptr<IAssetCollection>> s_AssetCollections;

    /** \brief Holds the threads currently loading assets */
    inline static std::vector<std::future<void>> s_AssetLoaders;

    /** \brief Pools the asset loader threads, and remove them if they're completed */
    static void PoolLoadingAssets();

    /** \brief Gets the asset collection holding the given asset type */
    template <typename T>
    static AssetCollection<T> &GetAssetCollection();

public:
    /** \brief Loads an asset by launching its builder function in a new thread */
    static void LoadAsset(std::future<void> builder);

    /** \brief Registers a new type of loadable asset */
    template <typename T>
    static AssetType Register();

    /** \brief Gets the asset of the following type, at the given file path */
    template <typename T>
    static T GetAsset(Path path);

    /** \brief Maps the given asset handle to its origin path */
    template <typename T>
    static void InsertAsset(Path path, T asset);

    /** \brief Removes an asset from the asset system */
    template <typename T>
    static void RemoveAsset(Path path);
};

template <typename T>
inline T AssetManager::AssetCollection<T>::getAsset(Path path)
{
    return collection[path];
}

template <typename T>
inline void AssetManager::AssetCollection<T>::insertAsset(Path path, T asset)
{
    collection.insert({path, asset});
}

template <typename T>
inline void AssetManager::AssetCollection<T>::removeAsset(Path path)
{
    collection.erase(path);
}

template <typename T>
inline AssetType AssetManager::Register()
{
    // New type ID is actually the position in the vector of the registered asset
    AssetType newType = s_AssetCollections.size();

#ifndef NDEBUG
    std::cout << "Registered asset " << boost::core::demangle(typeid(T).name()) << " with AssetType " << (int)newType << std::endl;
#endif

    // Create new collection
    s_AssetCollections.push_back(std::make_shared<AssetCollection<T>>());

    // Return type ID
    return newType;
}

template <typename T>
inline AssetManager::AssetCollection<T> &AssetManager::GetAssetCollection()
{
    // Since type ID is actually the position of the collection in the collection vector,
    // We can simply retrieve it like so
    return static_cast<AssetManager::AssetCollection<T> &>(*s_AssetCollections[T::AssetTypeID()]);
}

template <typename T>
inline T AssetManager::GetAsset(Path path)
{
    return GetAssetCollection<T>().getAsset(path);
}

template <typename T>
inline void AssetManager::InsertAsset(Path path, T asset)
{
    return GetAssetCollection<T>().insertAsset(path, asset);
}

template <typename T>
inline void AssetManager::RemoveAsset(Path path)
{
    GetAssetCollection<T>().removeAsset(path);
}
