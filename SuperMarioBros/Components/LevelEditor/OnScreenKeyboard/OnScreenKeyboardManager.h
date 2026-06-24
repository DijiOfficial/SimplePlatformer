#pragma once
#include <memory>

#include "Engine/Components/Component.h"

namespace onScreenKeyboard
{
    class OnScreenKey;

    class OnScreenKeyboardManager final : public diji::Component
    {
    public:
        explicit OnScreenKeyboardManager(diji::GameObject* ownerPtr) : Component{ ownerPtr } {}
        ~OnScreenKeyboardManager() noexcept override = default;

        void Init() override;
        void OnEnable() override {}
        void Start() override {}
        
        void Update() override {}
        void FixedUpdate() override {}
        void LateUpdate() override {}

        void OnDisable() override {}
        void OnDestroy() override {}

        void Move(const sf::Vector2f& direction);
        [[nodiscard]] char ConfirmSelection() const;

        // void Backspace();
        void Toggle() const;

    private:
        std::unique_ptr<diji::GameObject> m_KeyTemplateUPtr = nullptr;
        diji::GameObject* m_KeyboardRef = nullptr;
        diji::GameObject* m_Selector = nullptr;
        const OnScreenKey* m_CurrentKeyPtr = nullptr;
        // std::string m_CurrentText;
        // initial repeat delay: 180ms
        // repeat interval: 70ms
        using KeyboardLayout = std::vector<std::vector<char>>;
        using KeyGrid = std::vector<std::vector<OnScreenKey*>>;
        KeyGrid m_KeyboardGrid;// not needed?

        const float KEY_WIDTH   = 64.f;
        const float KEY_HEIGHT  = 64.f;

        const float HORIZONTAL_SPACING = 8.f;
        const float VERTICAL_SPACING   = 8.f;

        const sf::Vector2f START_POSITION = { 0.f, 100.f };
        
        enum : char
        {
            KeyShift      = '\1',
            KeyBackspace  = '\2',
            KeySpace      = '\3',
            KeyEnter      = '\4',
            KeySymbols    = '\5'
        };
        
        KeyboardLayout KEYBOARD_DATA =
        {
            {
                'Q', 'W', 'E', 'R', 'T',
                'Y', 'U', 'I', 'O', 'P'
            },

            {
                'A', 'S', 'D', 'F', 'G',
                'H', 'J', 'K', 'L'
            },

            {
                'Z', 'X', 'C', 'V',
                'B', 'N', 'M',
            },
        };

        void CreateItemTemplate();
        KeyGrid BuildKeyboard();

        void LinkNeighbours() const;
        void LinkHorizontal() const;
        void LinkVertical() const;
        OnScreenKey* FindVerticalCandidate(int startRow, int direction, const sf::Vector2f& origin) const;
        OnScreenKey* FindBestByPosition(const std::vector<OnScreenKey*>& row, const sf::Vector2f& origin) const;

        static int Wrap(const int num, const int max) { return (num + max) % max; }
    };
}
