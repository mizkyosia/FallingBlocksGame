#include <managers/ResourceManager.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>

std::queue<AssetsManager::AssetInfo<sf::Texture>> AssetsManager::m_textureQueue;
std::queue<AssetsManager::AssetInfo<sf::SoundBuffer>> AssetsManager::m_soundQueue;
std::queue<AssetsManager::AssetInfo<sf::Shader>> AssetsManager::m_shaderQueue;
std::queue<AssetsManager::AssetInfo<sf::Font>> AssetsManager::m_fontQueue;

std::map<std::filesystem::path, std::shared_ptr<sf::Texture>> AssetsManager::m_textureMap;
std::map<std::filesystem::path, std::shared_ptr<sf::SoundBuffer>> AssetsManager::m_soundMap;
std::map<std::filesystem::path, std::shared_ptr<sf::Shader>> AssetsManager::m_shaderMap;
std::map<std::filesystem::path, std::shared_ptr<sf::Font>> AssetsManager::m_fontMap;

AssetsManager::AssetsManager() : m_loadingThread([&]()
                                                 { loadAssets(); }),
                                 m_join(false), m_assetsMutex()
{
}

AssetsManager::~AssetsManager()
{
    // Stop thread, and wait for it to finish
    m_join = true;
    m_loadingThread.join();
}

void AssetsManager::loadAssets()
{
    // Forever running thread (except when quitting the game :P)
    while (!m_join)
    {
        // Fetch the assets, one at a time
        // Might become even more optimized through multiple threads (futures & such), but for now idc it's enough
        if (!m_textureQueue.empty())
            loadAsset(m_textureQueue);
        else if (!m_soundQueue.empty())
            loadAsset(m_soundQueue);
        else if (!m_fontQueue.empty())
            loadAsset(m_fontQueue);
        else if (!m_shaderQueue.empty())
            loadAsset(m_shaderQueue);
    }
}

std::shared_ptr<sf::Texture> AssetsManager::getTexture(std::filesystem::path path)
{
    return _getAsset<sf::Texture>(path, m_textureQueue, m_textureMap, [=]()
                                  { return sf::Texture(path); });
}

std::shared_ptr<sf::SoundBuffer> AssetsManager::getSound(std::filesystem::path path)
{
    return _getAsset<sf::SoundBuffer>(path, m_soundQueue, m_soundMap, [=]()
                                      { return sf::SoundBuffer(path); });
}

std::shared_ptr<sf::Font> AssetsManager::getFont(std::filesystem::path path)
{
    return _getAsset<sf::Font>(path, m_fontQueue, m_fontMap, [=]()
                               { return sf::Font(path); });
}

std::shared_ptr<sf::Shader> AssetsManager::getShader(std::filesystem::path path, sf::Shader::Type shaderType)
{
    return _getAsset<sf::Shader>(path, m_shaderQueue, m_shaderMap, [=]()
                                 { return sf::Shader(path, shaderType); });
}
