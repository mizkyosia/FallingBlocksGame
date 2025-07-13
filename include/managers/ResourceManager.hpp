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

class App;

class AssetsManager
{
public:
    template <typename T>
    struct AssetInfo
    {
        std::shared_ptr<T> memory;
        std::function<T(void)> instantiator;
    };

    static std::shared_ptr<sf::Texture> getTexture(std::filesystem::path path);
    static std::shared_ptr<sf::SoundBuffer> getSound(std::filesystem::path path);
    static std::shared_ptr<sf::Font> getFont(std::filesystem::path path);
    static std::shared_ptr<sf::Shader> getShader(std::filesystem::path path, sf::Shader::Type shaderType);

private:
    friend class App;

    AssetsManager();
    ~AssetsManager();

    std::thread m_loadingThread;
    std::atomic<bool> m_join;
    std::mutex m_assetsMutex;

    static std::queue<AssetInfo<sf::Texture>> m_textureQueue;
    static std::queue<AssetInfo<sf::SoundBuffer>> m_soundQueue;
    static std::queue<AssetInfo<sf::Shader>> m_shaderQueue;
    static std::queue<AssetInfo<sf::Font>> m_fontQueue;

    static std::map<std::filesystem::path, std::shared_ptr<sf::Texture>> m_textureMap;
    static std::map<std::filesystem::path, std::shared_ptr<sf::SoundBuffer>> m_soundMap;
    static std::map<std::filesystem::path, std::shared_ptr<sf::Shader>> m_shaderMap;
    static std::map<std::filesystem::path, std::shared_ptr<sf::Font>> m_fontMap;

    void loadAssets();

    template <typename T>
    void loadAsset(std::queue<AssetInfo<T>> &assetQueue);

    template <typename T>
    static std::shared_ptr<T> _getAsset(std::filesystem::path path, std::queue<AssetInfo<T>> &assetQueue, std::map<std::filesystem::path, std::shared_ptr<T>> &assetMap, std::function<T(void)> instantiator);
};

template <typename T>
inline void AssetsManager::loadAsset(std::queue<AssetInfo<T>> &assetQueue)
{
    // Fetch asset informations & remove them from queue
    AssetInfo<T> assetInfo = assetQueue.front();
    assetQueue.pop();
    // Load our asset (does not block since we're in another thread)
    T temp = assetInfo.instantiator();
    // Scoping for locking
    {
        std::lock_guard guard(m_assetsMutex);
        // Move the asset to its new place (minimal downtime)
        *assetInfo.memory = std::move(temp);
    }
    // And free the lock !
}

template <typename T>
inline std::shared_ptr<T> AssetsManager::_getAsset(std::filesystem::path path, std::queue<AssetInfo<T>> &assetQueue, std::map<std::filesystem::path, std::shared_ptr<T>> &assetMap, std::function<T(void)> instantiator)
{
    // If the asset has already been registered, then simply return a reference to it
    // It may not be loaded yet, though
    if (assetMap.contains(path))
        return assetMap[path];

    // Otherwise, create the shared pointer for the asset
    std::shared_ptr<T> ptr(nullptr);

    // Then queue it
    assetQueue.push(AssetInfo<T>{
        .memory = ptr,
        .instantiator = instantiator});

    // And finally return it !
    return ptr;
}
