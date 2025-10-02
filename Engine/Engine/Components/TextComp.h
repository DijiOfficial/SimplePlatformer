#pragma once
#include "Component.h"
#include <string>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Text.hpp>

namespace sf
{
    class Color;
}

namespace diji 
{
    // todo: add a scale member variable.
    class TextComp final : public Component
    { 
    public:
        explicit TextComp(GameObject* ownerPtr, const std::string& text, const std::string& filename, const sf::Color& color = { 255, 255, 255, 255 }, bool isCentered = false);
        ~TextComp() noexcept override = default;

        TextComp(const TextComp& other) = delete;
        TextComp(TextComp&& other) = delete;
        TextComp& operator=(const TextComp& other) = delete;
        TextComp& operator=(TextComp&& other) = delete;

        void Init() override {}
        void OnEnable() override {}
        void Start() override {}
		
        void FixedUpdate() override {}
        void Update() override {}
        void LateUpdate() override {}

        void OnDisable() override {}
        void OnDestroy() override {}

        void SetFont(const sf::Font& font);
        [[nodiscard]] sf::Text& GetText() { return m_Text; }
        void SetCentered(const bool isCentered) { m_IsCentered = isCentered; }
        [[nodiscard]] bool GetIsCentered() const { return m_IsCentered; }

    private:
        sf::Font m_Font;
        sf::Text m_Text;
        bool m_IsCentered = false;
    };
}
