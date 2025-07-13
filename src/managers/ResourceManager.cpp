#include <managers/ResourceManager.hpp>

AssetsManager::AssetsManager() : m_loadingThread([&]()
                                                 { loadAssets(); })
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