#pragma once
#include <SFML/Graphics/Font.hpp>

#include <assets/Asset.hpp>

namespace Assets
{
    struct Font : public Asset<Font, sf::Font>
    {
        Font(Path path) : Asset<Font, sf::Font>(path, [=]()
                                                { return sf::Font{path}; }) {};
    };

} // namespace Assets
