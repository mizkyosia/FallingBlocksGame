#include <managers/AssetManager.hpp>
#include <chrono>
#include <algorithm>

void AssetManager::PoolLoadingAssets()
{
    // Remove all loaded asset futures
    auto filter = std::remove_if(
        s_AssetLoaders.begin(),
        s_AssetLoaders.end(),
        [](std::future<void> &loader)
        {
            auto status = loader.wait_for(std::chrono::nanoseconds(0));
            if (status != std::future_status::timeout)
            {
                try
                {
                    loader.get();
                }
                catch (const std::exception &e)
                {
                    std::cerr << e.what() << '\n';
                }
                return true;
            }
            return false;
        });
    s_AssetLoaders.erase(
        filter, s_AssetLoaders.end());
}

void AssetManager::LoadAsset(std::future<void> builder)
{
    s_AssetLoaders.push_back(std::move(builder));
}
