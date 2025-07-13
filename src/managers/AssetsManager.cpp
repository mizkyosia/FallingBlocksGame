#include <managers/AssetsManager.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>

std::mutex AssetsManager::m_assetsMutex;

std::queue<AssetsManager::AssetInfo<sf::Texture>> AssetsManager::TextureQueue;
std::queue<AssetsManager::AssetInfo<sf::SoundBuffer>> AssetsManager::SoundQueue;
std::queue<AssetsManager::AssetInfo<sf::Shader>> AssetsManager::ShaderQueue;
std::queue<AssetsManager::AssetInfo<sf::Font>> AssetsManager::FontQueue;

std::map<std::filesystem::path, Asset<sf::Texture>> AssetsManager::TextureMap;
std::map<std::filesystem::path, Asset<sf::SoundBuffer>> AssetsManager::SoundMap;
std::map<std::filesystem::path, Asset<sf::Shader>> AssetsManager::ShaderMap;
std::map<std::filesystem::path, Asset<sf::Font>> AssetsManager::FontMap;

sf::Texture DefaultTexture{"assets/images/teto.png"};
sf::SoundBuffer DefaultSound{"assets/sounds/meow.mp3"};
sf::Font DefaultFont{"assets/fonts/VCR_OSD_MONO.ttf"};
sf::Shader DefaultShader{std::filesystem::path{"assets/shaders/button.frag"}, sf::Shader::Type::Fragment};

AssetsManager::AssetsManager() : m_loadingThread([&]()
                                                 { loadAssets(); }),
                                 m_join(false)
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
        if (!TextureQueue.empty())
            _loadAsset(TextureQueue);
        else if (!SoundQueue.empty())
            _loadAsset(SoundQueue);
        else if (!FontQueue.empty())
            _loadAsset(FontQueue);
        else if (!ShaderQueue.empty())
            _loadAsset(ShaderQueue);
    }
}

Asset<sf::Texture> AssetsManager::getTexture(std::filesystem::path path)
{
    return _getAsset<sf::Texture>(path, TextureQueue, TextureMap, [=](const std::filesystem::path &path)
                                  { return sf::Texture(path); });
}

Asset<sf::SoundBuffer> AssetsManager::getSound(std::filesystem::path path)
{
    return _getAsset<sf::SoundBuffer>(path, SoundQueue, SoundMap, [=](const std::filesystem::path &path)
                                      { return sf::SoundBuffer(path); });
}

Asset<sf::Font> AssetsManager::getFont(std::filesystem::path path)
{
    return _getAsset<sf::Font>(path, FontQueue, FontMap, [=](const std::filesystem::path &path)
                               { return sf::Font(path); });
}

Asset<sf::Shader> AssetsManager::getShader(std::filesystem::path path, sf::Shader::Type shaderType)
{
    return _getAsset<sf::Shader>(path, ShaderQueue, ShaderMap, [=](const std::filesystem::path &path)
                                 { return sf::Shader(path, shaderType); });
}
