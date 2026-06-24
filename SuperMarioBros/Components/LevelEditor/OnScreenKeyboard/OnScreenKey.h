#pragma once
#include "Engine/Components/Component.h"

namespace diji
{
    class TextComp;
}

namespace onScreenKeyboard
{
    class OnScreenKey final : public diji::Component
    {
    public:
        explicit OnScreenKey(diji::GameObject* ownerPtr) : Component{ ownerPtr }, m_Key{ 0 } {}

        ~OnScreenKey() noexcept override = default;

        void Init() override;
        void OnEnable() override {}
        void Start() override {}
        
        void Update() override {}
        void FixedUpdate() override {}
        void LateUpdate() override {}

        void OnDisable() override {}
        void OnDestroy() override {}

        enum class Direction : uint8_t
        {
            Up,
            Down,
            Left,
            Right,
            Count
        };
        const OnScreenKey* GetNeighbour(const Direction direction) const { return m_Neighbours[static_cast<size_t>(direction)]; }
        void SetNeighbour(Direction dir, const OnScreenKey* key) { m_Neighbours[static_cast<size_t>(dir)] = key; }

        void SetKey(const char key);
        [[nodiscard]] char PressKey() const { return m_Key; }
        
    private:
        const OnScreenKey* m_Neighbours[static_cast<size_t>(Direction::Count)] {};
        diji::TextComp* m_TextCompPtr = nullptr;
        char m_Key;
    };
}
