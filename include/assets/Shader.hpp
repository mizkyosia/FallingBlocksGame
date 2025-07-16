#pragma once
#include <SFML/Graphics/Shader.hpp>

#include <assets/Asset.hpp>

namespace Assets
{
    struct Shader : public Asset<Shader, sf::Shader>
    {
        DECLARE_ASSET()

        Shader(Path path, sf::Shader::Type shaderType) : Asset<Shader, sf::Shader>(path, [=]()
                                                                                   { return sf::Shader{path, shaderType}; }) {};
    };

} // namespace Assets
