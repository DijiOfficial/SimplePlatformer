#pragma once
#include "../Interfaces/EventSystem.h" // Included for all inherited classes

namespace diji 
{
    class GameObject;

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

    protected:
        explicit Component(GameObject* ownerPtr) : m_OwnerPtr{ ownerPtr } {}
        [[nodiscard]] const GameObject* GetOwner() const { return m_OwnerPtr; }

    private:
        GameObject* m_OwnerPtr = {};
    };
}
