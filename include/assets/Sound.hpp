#pragma once
#include <SFML/Audio/SoundBuffer.hpp>

#include <assets/Asset.hpp>

namespace Assets
{
    struct Sound : public Asset<Sound, sf::SoundBuffer>
    {
        Sound(Path path) : Asset<Sound, sf::SoundBuffer>(path, [=]()
                                                         { return sf::SoundBuffer{path}; }) {};
    };

} // namespace Assets
