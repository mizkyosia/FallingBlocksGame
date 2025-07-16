#pragma once
#include <vector>
#include <unordered_map>
#include <filesystem>
#include <mutex>
#include <future>

#include "Entity.hpp"

/**
 * \brief Scene struct, for easier loading of entities, systems, components & assets.
 *
 * The main objective of this structure is to be serialized/deserialized very easily, for game editing
 * 
 * It is not really used in the game, except for debugging & editing purposes
 */
struct Scene
{
private:
    /** \brief All scenes currently loaded into memory. They might not be "ready" yet though */
    static std::unordered_map<std::string, Scene> s_LoadedScenes;

    /** \brief Vector holding all the loading threads of scenes */
    static std::vector<std::future<Scene(std::filesystem::path)>> s_LoadingScenes;

public:
    std::string name;
    std::vector<Entity> entities;

    static void Load(std::string name);
    static void Unload(std::string name);
};