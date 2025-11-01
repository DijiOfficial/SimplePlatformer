#pragma once
#include "Engine/Components/Component.h"
#include "../../Interfaces/IPlayerBumpable.h"
#include "../../Helpers/MarioHelpers.h"

namespace diji
{
    class Timeline;
    class Collider;
}

namespace thomasWasLate
{
    class BaseBlock : public diji::Component, public IPlayerBumpable
    {
    public:
        enum class ItemSpawnType : uint8_t;
        explicit BaseBlock(diji::GameObject* ownerPtr, ItemSpawnType itemSpawnType, std::string audioFileName = "",  bool shouldSwitch = true);
        ~BaseBlock() noexcept override = default;

        void Init() override;
        void OnEnable() override {}
        void Start() override {}
        
        void Update() override {}
        void FixedUpdate() override {}
        void LateUpdate() override {}

        void OnDisable() override {}
        void OnDestroy() override {}

        void Bump() override;

        void SetAudioName(const std::string& audioName) { m_AudioName = audioName; }
        void SetShouldSwitchOnHit(const bool shouldSwitch) { m_ShouldSwitchOnHit = shouldSwitch; }

        // todo: I think I can eliminate this enum entirely. Also possible to decouple/split what a block spawns from its behaviour?
        enum class ItemSpawnType : uint8_t
        {
            None = 255,
            Coin = 0,
            PowerUp = 1,
            StarPowerUp = 2,
            OneUpMushroom = 3,
        };
        
    protected:
        std::unique_ptr<diji::GameObject> m_ItemTemplateUPtr = nullptr;
        diji::Timeline* m_TimelinePtr = nullptr;
        diji::Collider* m_ColliderCompPtr = nullptr;
        diji::Transform* m_TransformCompPtr = nullptr;
        std::string m_AudioName;
        ItemSpawnType m_ItemSpawnType = ItemSpawnType::None;
        bool m_IsHit = false;
        bool m_ShouldSwitchOnHit = true;

        virtual void OnAnimationStart();
        virtual void OnAnimationEnd() {}
        // virtual void OnCustomBumpLogic() {}
        virtual void CreateItemTemplate() {}
        
        void CreateTimeline();
        void PlayAnimation() const;
        void SwitchToEmptyBlockState();
    };
}
