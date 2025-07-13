#pragma once
#include <queue>
#include <thread>
#include <bits/shared_ptr.h>
#include <atomic>
#include <filesystem>
#include <map>
#include <mutex>
#include <future>

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

class App;

class AssetsManager
{
public:
    template <typename T>
    struct AssetInfo
    {
        std::shared_ptr<T> memory;
        std::filesystem::path path;
    };

    static void loadTexture();

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

    static std::map<std::filesystem::path, sf::Texture> m_textureMap;
    static std::map<std::filesystem::path, sf::SoundBuffer> m_soundMap;
    static std::map<std::filesystem::path, sf::Shader> m_shaderMap;
    static std::map<std::filesystem::path, sf::Font> m_fontMap;


    void loadAssets();

    template <typename T>
    void loadAsset(std::queue<AssetInfo<T>> &assetQueue);
};

template <typename T>
inline void AssetsManager::loadAsset(std::queue<AssetInfo<T>> &assetQueue)
{
    // Fetch asset informations & remove them from queue
    AssetInfo<T> assetInfo = assetQueue.front();
    assetQueue.pop();
    // Load our asset (does not block since we're in another thread)
    T temp(texInfo.path);
    // Scoping for locking
    {
        std::lock_guard guard(m_assetsMutex);
        // Move the asset to its new place (minimal downtime)
        *assetInfo.memory = std::move(temp);
    }
    // And free the lock !
}
