#pragma once
#include <assets/AssetManager.hpp>

template <typename Data>
template <typename... Args>
Asset<Data> AssetManager::AssetCollection<Data>::getAsset(AssetManager &manager, Path path, Args... args)
{
    auto it = collection.find(path);
    if (it != collection.end())
        return collection[path];

    // Start loading the asset if it doesn't exist
    Asset<Data> newAsset;

    // Insert a copy of this new asset
    collection.insert({path, Asset<Data>(newAsset)});

    manager.s_AssetLoaders.push_back(std::async(std::launch::async,

                                                [=]()
                                                {
                                                    // Build data
                                                    Data data(path, std::forward<Args>(args)...);
                                                    // Locking shenanigans for access through other threads
                                                    std::mutex mut;
                                                    {
                                                        std::lock_guard lock(mut);
                                                        // Move the data inside the handle
                                                        newAsset.m_handle = std::move(data);
                                                        // Mark the asset as ready
                                                        newAsset.m_loaded = true;
                                                        // Call the loaded "event"
                                                        newAsset.whenLoaded();
                                                    }
                                                }));
}

template <typename Data>
inline AssetType AssetManager::registerAsset()
{
    // New type ID is actually the position in the vector of the registered asset
    AssetType newType = getAssetTypeID<Data>();

#ifndef NDEBUG
    std::cout << "Registered asset " << boost::core::demangle(typeid(Data).name()) << " with AssetType " << (int)newType << std::endl;
#endif

    // Create new collection
    m_assetCollections.push_back(std::make_shared<AssetCollection<Data>>());

    // Return type ID
    return newType;
}

template <typename T>
inline AssetManager::AssetCollection<T> &AssetManager::getAssetCollection()
{
    // Since type ID is actually the position of the collection in the collection vector,
    // We can simply retrieve it like so
    return static_cast<AssetManager::AssetCollection<T> &>(*m_assetCollections[T::AssetTypeID()]);
}

template <typename T>
inline AssetType AssetManager::getAssetTypeID()
{
    static AssetType id = m_nextAssetType++;
    return id;
}

template <typename T>
inline Asset<T> AssetManager::get(Path path)
{
    return getAssetCollection<T>().getAsset(path);
}
