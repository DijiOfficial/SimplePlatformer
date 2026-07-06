#pragma once
#include "../Interfaces/EventSystem.h" // Included for all inherited classes
#include "../Collision/CollisionStructs.h"
#include "../Singleton/TimerManager.h"

namespace diji 
{
    class GameObject;
    class Collider;

    class Component
    {
        friend class Timer;
    public:
        virtual ~Component();

        Component(const Component& other) = delete;
        Component(Component&& other) = delete;
        Component& operator=(const Component& other) = delete;
        Component& operator=(Component&& other) = delete;

        // todo: they don't need to be pure virtual.
        virtual void Init() = 0;
        virtual void OnEnable() = 0;
        virtual void Start() = 0;
		
        virtual void FixedUpdate() = 0;
        virtual void Update() = 0;
        virtual void LateUpdate() = 0;

        virtual void OnDisable() = 0;
        virtual void OnDestroy() = 0;

        // these can easily be expanded to include CollisionInfo if needed.
        // todo: rename to OnOverlapEnter/Stay/Exit
        virtual void OnTriggerEnter(const Collider*, const CollisionInfo&) {}
        virtual void OnTriggerStay(const Collider*, const CollisionInfo&) {}
        virtual void OnTriggerExit(const Collider*, const CollisionInfo&) {}

        virtual void OnHitEvent(const Collider*, const CollisionInfo&) {}
        [[nodiscard]] const GameObject* GetOwner() const { return m_OwnerPtr; }
        void SetActive(bool isActive) const;
        void Destroy() const;
        void Destroy(float lifeTime) const;
        void SetRenderLayer(int layer) const;
        
    protected:
        explicit Component(GameObject* ownerPtr) : m_OwnerPtr{ ownerPtr } {}

    private:
        GameObject* m_OwnerPtr = {};
        mutable std::unordered_set<TimerManager::TimerHandle> m_TimerHandles;

        void RegisterTimerHandle(const TimerManager::TimerHandle& handle) const { m_TimerHandles.insert(handle); }
        void DeregisterTimerHandle(const TimerManager::TimerHandle& handle) const { m_TimerHandles.erase(handle); }
    };
}
