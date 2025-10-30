#pragma once
#include "Engine/Components/Component.h"
#include "Engine/Singleton/TimerManager.h"

namespace thomasWasLate
{
    class CastleFlag final : public diji::Component
    {
    public:
        explicit CastleFlag(diji::GameObject* ownerPtr) : Component{ ownerPtr } {}
        ~CastleFlag() noexcept override = default;

        void Init() override;
        void OnEnable() override {}
        void Start() override {}
        
        void Update() override;
        void FixedUpdate() override {}
        void LateUpdate() override {}

        void OnDisable() override {}
        void OnDestroy() override {}

        diji::Event<> OnLevelEndAnimationFinishedEvent;

    private:
        diji::TimerManager::TimerHandle m_TimerHandle{ std::numeric_limits<std::size_t>::max() };
        int m_FireworksToSpawn = 0;
        bool m_LastFireworkSpawnedIsOnLeftSide = false;
        bool m_EventWasTriggered = false;
        
        void StartAnimation(const int fireworksToSpawn);
        void SpawnFirework();
    };
}
