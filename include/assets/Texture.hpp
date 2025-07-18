#pragma once
#include <SFML/Graphics/Texture.hpp>

#include <assets/Asset.hpp>

namespace Assets
{
    struct Texture : public Asset<Texture, sf::Texture>
    {
        DECLARE_ASSET()

        Texture(Path path) : Asset<Texture, sf::Texture>(path, [=]()
                                                         {
                                                    // SFML loads the texture
                                                    return sf::Texture{path}; }) {};

        void whenLoaded() override
        {
        }
    };

} // namespace Assets
