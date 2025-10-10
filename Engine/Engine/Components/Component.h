#pragma once
#include "../Interfaces/EventSystem.h" // Included for all inherited classes
#include "../Collision/CollisionStructs.h"

namespace diji 
{
    class GameObject;
    class Collider;

    class Component
    {
    public:
        virtual ~Component() noexcept = default;

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
        virtual void OnTriggerEnter(const Collider*) {}
        virtual void OnTriggerStay(const Collider*) {}
        virtual void OnTriggerExit(const Collider*) {}

        virtual void OnHitEvent(const Collider*, const CollisionInfo&) {}

    protected:
        explicit Component(GameObject* ownerPtr) : m_OwnerPtr{ ownerPtr } {}
        [[nodiscard]] const GameObject* GetOwner() const { return m_OwnerPtr; }

    private:
        GameObject* m_OwnerPtr = {};
    };
}
