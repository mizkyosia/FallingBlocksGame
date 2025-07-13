#pragma once
#include <queue>
#include <thread>
#include <bits/shared_ptr.h>
#include <atomic>
#include <filesystem>
#include <map>
#include <mutex>
#include <functional>

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Audio/SoundBuffer.hpp>

#include <util/Asset.hpp>

class App;

class AssetsManager
{
public:
    template <typename T>
    struct AssetInfo
    {
        Asset<T> memory;
        std::filesystem::path path;
        std::function<T(const std::filesystem::path&)> builder;
    };

    static sf::Texture DefaultTexture;
    static sf::SoundBuffer DefaultSound;
    static sf::Font DefaultFont;
    static sf::Shader DefaultShader;

    static Asset<sf::Texture> getTexture(std::filesystem::path path);
    static Asset<sf::SoundBuffer> getSound(std::filesystem::path path);
    static Asset<sf::Font> getFont(std::filesystem::path path);
    static Asset<sf::Shader> getShader(std::filesystem::path path, sf::Shader::Type shaderType);

private:
    friend class App;

    AssetsManager();
    ~AssetsManager();

    std::thread m_loadingThread;
    std::atomic<bool> m_join;

    static std::mutex m_assetsMutex;

    static std::queue<AssetInfo<sf::Texture>> TextureQueue;
    static std::queue<AssetInfo<sf::SoundBuffer>> SoundQueue;
    static std::queue<AssetInfo<sf::Shader>> ShaderQueue;
    static std::queue<AssetInfo<sf::Font>> FontQueue;

    static std::map<std::filesystem::path, Asset<sf::Texture>> TextureMap;
    static std::map<std::filesystem::path, Asset<sf::SoundBuffer>> SoundMap;
    static std::map<std::filesystem::path, Asset<sf::Shader>> ShaderMap;
    static std::map<std::filesystem::path, Asset<sf::Font>> FontMap;

    void loadAssets();

    template <typename T>
    static void _loadAsset(std::queue<AssetInfo<T>> &assetQueue);

    template <typename T>
    static Asset<T> _getAsset(std::filesystem::path path, std::queue<AssetInfo<T>> &assetQueue, std::map<std::filesystem::path, Asset<T>> &assetMap, std::function<T(const std::filesystem::path&)> builder);
};

template <typename T>
inline void AssetsManager::_loadAsset(std::queue<AssetInfo<T>> &assetQueue)
{
    // Fetch asset informations & remove them from queue
    AssetInfo<T> assetInfo = assetQueue.front();
    assetQueue.pop();
    // Load our asset (does not block since we're in another thread)
    T temp = assetInfo.builder(assetInfo.path);
    // Scoping for locking
    {
        std::lock_guard guard(m_assetsMutex);
        // Move the asset to its new place (minimal downtime)
        *assetInfo.memory.m_handle = std::move(temp);
    }
    // And free the lock !
    // Then, mark asset as loaded
    *assetInfo.memory.m_loaded = true;
}

template <typename T>
inline Asset<T> AssetsManager::_getAsset(std::filesystem::path path, std::queue<AssetInfo<T>> &assetQueue, std::map<std::filesystem::path, Asset<T>> &assetMap, std::function<T(const std::filesystem::path&)> builder)
{
    // If the asset has already been registered, then simply return a reference to it
    // It may not be loaded yet, though
    if (assetMap.contains(path))
        return assetMap[path];

    // Otherwise, create the handle for the asset
    Asset<T> handle;
    Asset<T> copy(handle);

    // Then queue it
    assetQueue.push(AssetInfo<T>{
        .memory = copy,
        .path = path,
        .builder = builder});

    // And finally return it !
    return copy;
}
