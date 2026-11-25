#pragma once
#include <SFML/Graphics/Texture.hpp>

namespace diji
{
    class TextureHandle final
    {
    public:
        TextureHandle() = default;
        explicit TextureHandle(const sf::Texture& texture) : m_TexturePtr{ &texture } {}
        ~TextureHandle() noexcept = default;

        TextureHandle(const TextureHandle& other) = default;
        TextureHandle(TextureHandle&& other) noexcept = default;
        TextureHandle& operator=(const TextureHandle& other) = default;
        TextureHandle& operator=(TextureHandle&& other) noexcept = default;

        [[nodiscard]] bool IsValid() const { return m_TexturePtr != nullptr; }
        [[nodiscard]] const sf::Texture& Get() const { return *m_TexturePtr; }
        [[nodiscard]] sf::Vector2u GetSize() const { return IsValid() ? m_TexturePtr->getSize() : sf::Vector2u{ 0, 0 }; }

    private:
        const sf::Texture* m_TexturePtr = nullptr;
    };
}
