#include "OneUpMushroom.h"

#include "../../Singletons/GameManager.h"
#include "Engine/Collision/Collider.h"
#include "Engine/Singleton/SceneManager.h"

void thomasWasLate::OneUpMushroom::Start()
{
    MushroomScript::Start();

    m_PointString = "1UP";
    m_ColliderCompPtr->SetTag("1upMushroom");

    m_ColliderCompPtr->OverlapCollider(diji::SceneManager::GetInstance().GetGameObject("X_PlayerChar")->GetComponent<diji::Collider>());
}

void thomasWasLate::OneUpMushroom::OnHitEvent(const diji::Collider* collider, const diji::CollisionInfo& collision_info)
{
    MushroomScript::OnHitEvent(collider, collision_info);

    if (collider->GetTag() == "player")
        GameManager::GetInstance().AddLife();
}
