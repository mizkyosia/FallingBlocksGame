#include <managers/AssetManager.hpp>
#include <chrono>
#include <algorithm>

void AssetManager::PoolLoadingAssets()
{
    // Remove all loaded assets
    s_AssetLoaders.erase(std::remove_if(s_AssetLoaders.begin(), s_AssetLoaders.end(), [&](const std::future<void> &loader)
                                        { return loader.wait_for(std::chrono::seconds(0)) == std::future_status::ready; }));
}

void AssetManager::LoadAsset(std::future<void> builder)
{
    s_AssetLoaders.push_back(std::move(builder));
}
