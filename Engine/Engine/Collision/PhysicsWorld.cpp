#include "PhysicsWorld.h"
#include "Collider.h"
#include "CollisionDispatcher.h"
#include "../Singleton/Helpers.h"
#include "../Core/GameObject.h"
#include "QuadTree.h"

#include <stdexcept>

void diji::PhysicsWorld::Reset()
{
    m_DynamicColliders = std::vector<Collider*>();
    m_StaticInfos = std::vector<StaticColliderInfo>();
    m_ActiveTriggers = std::vector<TriggerPair>();
    m_PreviousFrameTriggers = std::vector<TriggerPair>();
    m_HitEventTriggers = std::vector<TriggerPair>();
    m_WorldBounds = sf::FloatRect();
    m_QuadTree = std::make_unique<QuadTree>(m_WorldBounds);
}

void diji::PhysicsWorld::AddCollider(Collider* collider)
{
    if (!collider)
        throw std::runtime_error("Ya done did fucked up");

    UpdateWorldBounds(collider->GetAABB());

    if (collider->IsStatic())
    {
        // Create lightweight static entry
        StaticColliderInfo info;
        info.aabb = collider->GetAABB();
        info.collider = collider;

        // Check for existing static collider?
        m_StaticInfos.emplace_back(info);
    }
    else
    {
        if (std::ranges::find(m_DynamicColliders, collider) == m_DynamicColliders.end())
            m_DynamicColliders.emplace_back(collider);
    }
}

void diji::PhysicsWorld::RemoveCollider(Collider* collider)
{
    std::erase(m_DynamicColliders, collider);

    const auto it = std::ranges::find_if(m_StaticInfos,
        [collider](const StaticColliderInfo& sColInfo)
        { 
            return sColInfo.collider == collider; 
        });

    if (it != m_StaticInfos.end())
        m_StaticInfos.erase(it);
        
    RemoveFromTriggerLists(collider);
}

void diji::PhysicsWorld::FixedUpdate()
{
    // Phase 1: Predict movement
    std::vector<Prediction> predictionsVec;
    PredictMovement(predictionsVec);

    // todo: Massive issue with rect colliders and composed ground colliders, where colliders moving along the ground get stopped on seams because the collider is getting pushed back instead of up
    // Phase 2: Detect collisions using predicted positions
    DetectCollisions(predictionsVec);

    // Phase 3: Resolve collisions and apply friction
    for (Prediction& prediction : predictionsVec)
    {
        for (const CollisionInfo& collision : prediction.collisionInfoVec)
        {
            if (!collision.hasCollision)
                continue;

            if (!prediction.collider->IsMoveable())
                continue;

            ResolveCollision(prediction, collision);
            // ApplyFriction(prediction, collision);
        }

        ApplyFrictionOnce(prediction);
    }

    // Phase 4: Update final state
    for (Prediction& prediction : predictionsVec)
    {
        if (!prediction.collider->IsMoveable())
            continue;
        
        UpdateFinalPosition(prediction);
    }

    ProcessTriggerEvents();
}

std::optional<diji::RaycastHit> diji::PhysicsWorld::Raycast(const sf::Vector2f& origin, const sf::Vector2f& direction, const float maxDistance, const Collider* collider) const
{
    std::optional<RaycastHit> closestHit;
    float closestDist = maxDistance;
    
    const sf::Vector2f invDir
    {
        direction.x == 0.0f ? std::numeric_limits<float>::max() : 1.0f / direction.x,
        direction.y == 0.0f ? std::numeric_limits<float>::max() : 1.0f / direction.y
    };

    auto testCollider = [&](const Collider* col, const sf::FloatRect& rect)
    {
        // Slab method: x slabs
        const float minX = rect.position.x;
        const float maxX = rect.position.x + rect.size.x;
        const float t1 = (minX - origin.x) * invDir.x;
        const float t2 = (maxX - origin.x) * invDir.x;
        float tEnter = std::min(t1, t2);
        float tExit  = std::max(t1, t2);

        // y slabs
        const float minY = rect.position.y;
        const float maxY = rect.position.y + rect.size.y;
        const float t3 = (minY - origin.y) * invDir.y;
        const float t4 = (maxY - origin.y) * invDir.y;
        
        tEnter = std::max(tEnter, std::min(t3, t4));
        tExit  = std::min(tExit,  std::max(t3, t4));

        if (tExit < 0.0f || tEnter > tExit) return;

        const float hitParam = (tEnter >= 0.0f ? tEnter : tExit);
        if (hitParam < 0.0f || hitParam > closestDist) return;

        const sf::Vector2f hitPoint = origin + direction * hitParam;
        
        const sf::Vector2f deltaToHit = hitPoint - origin;
        const float actualDistance = Helpers::LengthFast(deltaToHit);
        
        if (actualDistance > closestDist) return;

        const sf::Vector2f normal = col->GetSurfaceNormalAt(hitPoint);

        RaycastHit hit;
        hit.collider            = col;
        hit.info.point          = hitPoint;
        hit.info.normal         = normal;
        hit.distance            = actualDistance;  // Use actual pixel distance
        hit.info.hasCollision   = true;
        hit.info.penetration    = 0.0f;
        hit.info.tangent        = sf::Vector2f{0.0f, 0.0f};
        hit.info.normalImpulse  = 0.0f;
        
        closestDist = actualDistance;
        closestHit = hit;
    };

    // 1) Dynamic colliders
    for (const Collider* col : m_DynamicColliders)
    {
        if (!collider || col == collider || collider->IsIgnoringCollider(col))
            continue;

        testCollider(col, col->GetAABB());
    }

    // 2) Static colliders
    for (const auto& [aabb, colliderPtr] : m_StaticInfos)
    {
        if (!colliderPtr || !collider || colliderPtr == collider || collider->IsIgnoringCollider(colliderPtr))
            continue;

        // testCollider(colliderPtr, colliderPtr->GetShape()->GetLocalShapeBounds());
        testCollider(colliderPtr, aabb);
    }

    return closestHit;
}

void diji::PhysicsWorld::RemoveFromTriggerLists(Collider* collider)
{
    auto removeFromActive = [collider](const TriggerPair& pair)
    {
        return pair.trigger == collider || pair.other == collider;
    };
    std::erase_if(m_ActiveTriggers, removeFromActive);
    
    std::erase_if(m_PreviousFrameTriggers, removeFromActive);
    
    std::erase_if(m_HitEventTriggers, removeFromActive);
}

void diji::PhysicsWorld::ProcessTriggerEvents()
{
    for (const auto& trigger : m_ActiveTriggers)
    {
        if (std::ranges::find(m_PreviousFrameTriggers, trigger) == m_PreviousFrameTriggers.end())
        {
            NotifyTriggerEvent(trigger, EventType::Enter);
        }
    }
    
    for (const auto& trigger : m_PreviousFrameTriggers)
    {
        if (std::ranges::find(m_ActiveTriggers, trigger) == m_ActiveTriggers.end())
        {
            NotifyTriggerEvent(trigger, EventType::Exit);
        }
    }
    
    for (const auto& trigger : m_ActiveTriggers)
    {
        if (std::ranges::find(m_PreviousFrameTriggers, trigger) != m_PreviousFrameTriggers.end())
        {
            NotifyTriggerEvent(trigger, EventType::Stay);
        }
    }

    for (const auto& trigger : m_HitEventTriggers)
    {
        NotifyHitEvent(trigger, EventType::Hit);
    }

    // todo: could potentially allocate a lot of unused memory.
    m_PreviousFrameTriggers = m_ActiveTriggers;
    m_ActiveTriggers.clear();
    m_HitEventTriggers.clear();
}

void diji::PhysicsWorld::NotifyTriggerEvent(const TriggerPair& trigger, const EventType eventType)
{
    if (auto* triggerGameObject = trigger.trigger->GetParent())
        triggerGameObject->NotifyTriggerEvent(trigger.other, eventType, trigger.hitInfo);
    
    if (auto* otherGameObject = trigger.other->GetParent())
        otherGameObject->NotifyTriggerEvent(trigger.trigger, eventType, trigger.hitInfo);
}

void diji::PhysicsWorld::NotifyHitEvent(const TriggerPair& trigger, const EventType eventType)
{
    if (auto* triggerGameObject = trigger.trigger->GetParent())
        triggerGameObject->NotifyTriggerEvent(trigger.other, eventType, trigger.hitInfo);
}

void diji::PhysicsWorld::PredictMovement(std::vector<Prediction>& predictionsVec) const
{
    const float dt = m_TimeSingletonInstance.GetFixedUpdateDeltaTime();
    predictionsVec.reserve(m_DynamicColliders.size());

    for (auto* collider : m_DynamicColliders)
    {
        if (!collider) continue;
        if (!collider->IsActive()) continue;

        sf::Vector2f forcesApplied = collider->GetNetForce() / collider->GetMass();
        sf::Vector2f vel = collider->GetVelocity();
        if (collider->IsAffectedByGravity())
            forcesApplied += m_Gravity;

        vel += forcesApplied * dt;

        sf::Vector2f proposedPos = collider->GetParent()->GetObjectPosition() + vel * dt;
        const auto predictedAabb = collider->GetAABBAt(proposedPos);

        predictionsVec.push_back({ .collider= collider, .AABB= predictedAabb, .pos= proposedPos, .vel= vel, .collisionInfoVec={} });
    }
}

void diji::PhysicsWorld::DetectCollisions(std::vector<Prediction>& predictionsVec)
{
    // todo: I believe this can be multithreaded?
    const size_t& size = predictionsVec.size(); 
    for (size_t i = 0; i < size; ++i)
    {
        auto& [colliderPtr, predictedAABB, pos, vel, collisionsVec] = predictionsVec[i];
        if (!colliderPtr->IsActive()) continue;

        // STATIC COLLISIONS: Check against all static colliders
        for (const auto& [aabb, staticCollider] : m_StaticInfos)
        {
            if (!staticCollider->IsActive()) continue;
            if (colliderPtr->GetCollisionResponse() == Collider::CollisionResponse::Ignore) continue;
            if (colliderPtr->IsIgnoringCollider(staticCollider) || staticCollider->IsIgnoringCollider(colliderPtr)) continue;
            if (!AABBOverlap(predictedAABB, aabb)) continue;
            
            const auto [Overlap, Hit] = HandleStaticCollisions(predictionsVec[i], staticCollider);
            
            if (Overlap)
                m_ActiveTriggers.push_back({.trigger = colliderPtr, .other = staticCollider, .hitInfo = collisionsVec.back()});

            if (Hit && colliderPtr->IsGenerateHitEvents())
                m_HitEventTriggers.push_back({.trigger = colliderPtr, .other = staticCollider, .hitInfo = collisionsVec.back()});
        }

        // DYNAMIC COLLISIONS: Check against remaining dynamic colliders (avoid duplicates)
        for (size_t j = i + 1; j < size; ++j)
        {
            Prediction& otherPrediction = predictionsVec[j];
            if (!otherPrediction.collider->IsActive()) continue;
            if (colliderPtr->GetCollisionResponse() == Collider::CollisionResponse::Ignore || otherPrediction.collider->GetCollisionResponse() == Collider::CollisionResponse::Ignore) continue;
            if (colliderPtr->IsIgnoringAllDynamicColliders() || otherPrediction.collider->IsIgnoringAllDynamicColliders()) continue;
            if (colliderPtr->IsIgnoringCollider(otherPrediction.collider) || otherPrediction.collider->IsIgnoringCollider(colliderPtr)) continue;
            if (!AABBOverlap(predictedAABB, otherPrediction.AABB)) continue;
            
            const auto [Overlap, Hit] = HandleDynamicCollisions(predictionsVec[i], otherPrediction);
            
            if (Overlap)
                m_ActiveTriggers.push_back({.trigger = colliderPtr, .other = otherPrediction.collider, .hitInfo = collisionsVec.back()});

            if (Hit)
            {
                if (colliderPtr->IsGenerateHitEvents())
                    m_HitEventTriggers.push_back({.trigger = colliderPtr, .other = otherPrediction.collider, .hitInfo = collisionsVec.back()});
                
                if (otherPrediction.collider->IsGenerateHitEvents())
                    m_HitEventTriggers.push_back({.trigger = otherPrediction.collider, .other = colliderPtr, .hitInfo = otherPrediction.collisionInfoVec.back()});
            }
        }
    }
}

// todo: ultimately this model is too slow, from the creation of the quadtree to avoiding duplicate checks it doesn't work well
// void diji::PhysicsWorld::DetectCollisions(std::vector<Prediction>& predictionsVec)
// {
//     std::unordered_set<std::pair<const Collider*, const Collider*>, PairHash> testedPairs;
//     testedPairs.reserve(1024);
//     
//     if (!m_QuadTree)
//         m_QuadTree = std::make_unique<QuadTree>(m_WorldBounds);
//     
//     std::unordered_map<const Collider*, size_t> predictionIndex;
//     predictionIndex.reserve(predictionsVec.size());
//     for (size_t i = 0; i < predictionsVec.size(); ++i)
//     {
//         predictionIndex.emplace(predictionsVec[i].collider, i);
//     }
//
//     auto makePair = [](const Collider* a, const Collider* b)
//     {
//         return (a < b) ? std::make_pair(a, b) : std::make_pair(b, a);
//     };
//
//     // Query quadtree for clusters containing both dynamic and static colliders
//     const auto clusters = m_QuadTree->Query(m_DynamicColliders, m_StaticInfos);
//
//     // For each cluster, first test dynamic-dynamic pairs, then dynamic-static pairs
//     for (const auto& [dynamic, statics] : clusters)
//     {
//         // --- dynamic-dynamic (pairwise) ---
//         const size_t dynCount = dynamic.size();
//         for (size_t a = 0; a < dynCount; ++a)
//         {
//             const Collider* aCol = dynamic[a];
//             // find prediction index for a
//             const auto ita = predictionIndex.find(aCol);
//             if (ita == predictionIndex.end()) continue; // not a dynamic prediction (defensive)
//             Prediction& predA = predictionsVec[ita->second];
//             if (!predA.collider->IsActive()) continue;
//
//             for (size_t b = a + 1; b < dynCount; ++b)
//             {
//                 const Collider* bCol = dynamic[b];
//                 const auto itb = predictionIndex.find(bCol);
//                 if (itb == predictionIndex.end()) continue;
//                 Prediction& predB = predictionsVec[itb->second];
//                 if (!predB.collider->IsActive()) continue;
//
//                 // Ignore rules and early outs (mirror your original checks)
//                 if (predA.collider->GetCollisionResponse() == Collider::CollisionResponse::Ignore ||
//                     predB.collider->GetCollisionResponse() == Collider::CollisionResponse::Ignore)
//                     continue;
//
//                 if (predA.collider->IsIgnoringAllDynamicColliders() || predB.collider->IsIgnoringAllDynamicColliders())
//                     continue;
//
//                 if (predA.collider->IsIgnoringCollider(predB.collider) || predB.collider->IsIgnoringCollider(predA.collider))
//                     continue;
//
//                 auto pair = makePair(aCol, bCol);
//                 if (!testedPairs.insert(pair).second)
//                     continue; // already processed this pair this frame
//                 
//                 if (!AABBOverlap(predA.AABB, predB.AABB))
//                     continue;
//
//                 // Narrow-phase dynamic-dynamic
//                 const auto [Overlap, Hit] = HandleDynamicCollisions(predA, predB);
//
//                 if (Overlap)
//                     m_ActiveTriggers.push_back({ .trigger = predA.collider, .other = predB.collider, .hitInfo = predA.collisionInfoVec.back() });
//
//                 if (Hit)
//                 {
//                     if (predA.collider->IsGenerateHitEvents())
//                         m_HitEventTriggers.push_back({ .trigger = predA.collider, .other = predB.collider, .hitInfo = predA.collisionInfoVec.back() });
//
//                     if (predB.collider->IsGenerateHitEvents())
//                         m_HitEventTriggers.push_back({ .trigger = predB.collider, .other = predA.collider, .hitInfo = predB.collisionInfoVec.back() });
//                 }
//             }
//         }
//
//         // --- dynamic-static ---
//         const size_t statCount = statics.size();
//         for (size_t i = 0; i < dynCount; ++i)
//         {
//             const Collider* dCol = dynamic[i];
//             const auto itPred = predictionIndex.find(dCol);
//             if (itPred == predictionIndex.end()) continue;
//             Prediction& pred = predictionsVec[itPred->second];
//             if (!pred.collider->IsActive()) continue;
//
//             // early-out: dynamic collider ignoring static response
//             if (pred.collider->GetCollisionResponse() == Collider::CollisionResponse::Ignore) continue;
//
//             for (size_t s = 0; s < statCount; ++s)
//             {
//                 const auto sCol = statics[s];
//                 if (!sCol.collider || !sCol.collider->IsActive()) continue;
//
//                 if (pred.collider->IsIgnoringCollider(sCol.collider) || sCol.collider->IsIgnoringCollider(pred.collider)) continue;
//
//                 auto pair = makePair(dCol, sCol.collider);
//                 if (!testedPairs.insert(pair).second)
//                     continue;
//                 
//                 if (!AABBOverlap(pred.AABB, sCol.aabb)) continue;
//
//                 // Narrow-phase dynamic-static
//                 const auto [Overlap, Hit] = HandleStaticCollisions(pred, sCol.collider);
//
//                 if (Overlap)
//                     m_ActiveTriggers.push_back({ .trigger = pred.collider, .other = sCol.collider, .hitInfo = pred.collisionInfoVec.back() });
//
//                 if (Hit && pred.collider->IsGenerateHitEvents())
//                     m_HitEventTriggers.push_back({ .trigger = pred.collider, .other = sCol.collider, .hitInfo = pred.collisionInfoVec.back() });
//             }
//         }
//     }
// }

void diji::PhysicsWorld::ResolveCollision(Prediction& prediction, const CollisionInfo& collision)
{
    const float velocityAlongNormal = Helpers::DotProduct(prediction.vel, collision.normal);
    
    // Objects separating? No collision response needed (technically not needed as I'm snapping instead of resolving over dt)
    if (velocityAlongNormal > 0) return;
    
    const float restitution = prediction.collider->GetRestitution();
    const float impulseScalar = -(1 + restitution) * velocityAlongNormal;
    const sf::Vector2f impulse = impulseScalar * collision.normal;
    prediction.vel += impulse;
    collision.normalImpulse = impulseScalar;

    constexpr float correctionSlop = 0.1f;    // Minimum penetration to correct
    if (collision.penetration > correctionSlop)
    {
        constexpr float correctionPercent = 0.8f;
        
        const sf::Vector2f correction = (collision.penetration - correctionSlop) * correctionPercent * collision.normal;
        prediction.pos += correction;
    }
    
    // Instead, just set position to the collision boundary
    // prediction.pos = collision.point;
}

// AI attempt at implementing Coulomb friction model with static and kinetic friction. I haven't checked thus I'm not using it
void diji::PhysicsWorld::ApplyFrictionOnceWithStaticKinetic(Prediction& prediction) const
{
    const float dt = m_TimeSingletonInstance.GetFixedUpdateDeltaTime();
    const float mass = prediction.collider->GetMass();
    if (mass <= 0.0f) return;

    // 1) Gather total normal impulse (impulse units: mass * deltaV)
    float totalNormalImpulse = 0.0f;
    sf::Vector2f weightedTangent{0.0f, 0.0f};

    bool anyCollision = false;
    for (const auto& collisionInfo : prediction.collisionInfoVec)
    {
        if (!collisionInfo.hasCollision) continue;
        anyCollision = true;
        const float absN = std::abs(collisionInfo.normalImpulse);
        totalNormalImpulse += absN;
        weightedTangent += collisionInfo.tangent * absN; 
    }

    if (!anyCollision) return;

    // If total normal impulse is essentially zero, fallback to simple frame-based budget
    // (This happens if ResolveCollision didn't set impulses for some reason)
    constexpr float eps = std::numeric_limits<float>::epsilon();
    if (totalNormalImpulse < eps)
    {
        // fallback: use force-based friction budget = mu * normalForce * dt
        const float gravityMag = std::abs(m_Gravity.y);
        const float normalForce = mass * gravityMag;
        const float muFallback = prediction.collider->GetKineticFriction();
        const float maxFrictionImpulseFallback = muFallback * normalForce * dt; // impulse units

        // Cancel velocity magnitude up to that budget (opposite direction of velocity)
        const float speed = Helpers::LengthFast(prediction.vel);
        if (speed <= 1e-6f) return;
        const sf::Vector2f velDir = prediction.vel / speed;
        const float neededImpulse = mass * speed; // impulse to stop
        const float appliedImpulse = std::min(maxFrictionImpulseFallback, neededImpulse);
        const sf::Vector2f deltaV = -(appliedImpulse / mass) * velDir;
        prediction.vel += deltaV;
        return;
    }

    // 2) Determine representative tangent direction
    const float tLen = Helpers::LengthFast(weightedTangent);
    sf::Vector2f repTangent;
    if (tLen > eps)
        repTangent = weightedTangent / tLen; // normalized
    else
    {
        // As fallback use current velocity's direction orthogonal to normal sum.
        const float speed = Helpers::LengthFast(prediction.vel);
        if (speed <= 1e-6f) return; // not moving tangentially
        // Direction opposite to velocity (we'll compute tangential later)
        repTangent = prediction.vel / speed;
    }

    // 3) Compute tangential velocity along representative tangent
    const float tangentialVelocity = Helpers::DotProduct(prediction.vel, repTangent);
    // small threshold: no significant tangential motion
    if (std::abs(tangentialVelocity) < 1e-5f) return;

    // 4) Friction budget (impulse) via Coulomb: maxFrictionImpulse = mu * totalNormalImpulse
    // Support both static and kinetic friction coefficients:
    // If your Collider only has one friction value, treat it as kinetic and compute static=1.5*mu_kinetic for a bit "stickiness".
    const float mu_k = prediction.collider->GetKineticFriction();       // kinetic
    float mu_s = prediction.collider->GetStaticFriction(); // static; ensure you add this accessor or fallback below

    // Fallback if collider doesn't expose static friction separately:
    if (mu_s <= 0.0f)
        mu_s = std::max(mu_k * 1.5f, mu_k); // a reasonable default (static >= kinetic)

    const float maxStaticImpulse = mu_s * totalNormalImpulse;
    const float maxKineticImpulse = mu_k * totalNormalImpulse;

    // 5) Determine needed impulse to stop tangential motion immediately:
    const float neededImpulse = std::abs(tangentialVelocity) * mass; // impulse = m * |v_t|

    float appliedImpulse;
    if (neededImpulse <= maxStaticImpulse)
    {
        // We can stop the tangential motion completely (static friction case)
        appliedImpulse = neededImpulse;
    }
    else
    {
        // Sliding (kinetic friction): apply kinetic friction impulse (opposes motion)
        appliedImpulse = maxKineticImpulse;
    }

    if (appliedImpulse <= 0.0f) return;

    // 6) Apply impulse opposite to tangential velocity:
    const float sign = (tangentialVelocity > 0.0f) ? -1.0f : 1.0f;
    const float deltaV = (appliedImpulse / mass) * sign;
    prediction.vel += repTangent * deltaV;
}

void diji::PhysicsWorld::ApplyFrictionOnce(Prediction& prediction) const
{
    const float mu = prediction.collider->GetStaticFriction();
    if (mu <= 0.0f) return;

    const float dt = m_TimeSingletonInstance.GetFixedUpdateDeltaTime();
    const float mass = prediction.collider->GetMass();
    if (mass <= 0.0f) return;

    bool hadCollision = false;
    bool hasGroundCollision = false;

    for (const auto& collisionInfo : prediction.collisionInfoVec)
    {
        if (!collisionInfo.hasCollision)
            continue;

        hadCollision = true;

        // Determine if this is a "ground" collision, assumes normal points upward
        if (collisionInfo.normal.y < -0.5f) // adjust threshold as needed
        {
            hasGroundCollision = true;
            break;
        }
    }

    if (!hadCollision)
        return;

    if (prediction.collider->IsOnlyApplyingGroundFriction() && !hasGroundCollision)
        return;

    const float speed = Helpers::LengthFast(prediction.vel);
    if (speed <= Helpers::EPSILON)
        return;

    const float gravityMagnitude = std::abs(m_Gravity.y);
    const float normalForce = mass * gravityMagnitude;
    const float frictionForce = mu * normalForce;
    const float maxFrictionImpulseThisFrame = frictionForce * dt;
    const float neededImpulse = mass * speed;
    const float appliedImpulse = std::min(maxFrictionImpulseThisFrame, neededImpulse);

    if (appliedImpulse <= 0.0f)
        return;

    const sf::Vector2f velDir = prediction.vel / speed;
    const sf::Vector2f deltaV = -(appliedImpulse / mass) * velDir;

    prediction.vel += deltaV;
}

void diji::PhysicsWorld::ApplyFriction(Prediction& prediction)
{
    // We treat normals with y < -threshold as ground contacts.
    constexpr float groundNormalThreshold = 0.7f; // cos ~ 45 degrees -> consider as ground
    const float mass = prediction.collider->GetMass();
    if (mass <= 0.0f) return;

    float totalNormalImpulse = 0.0f;
    sf::Vector2f avgTangent{0.0f, 0.0f};
    int groundContactCount = 0;

    for (const auto& collisionInfo : prediction.collisionInfoVec)
    {
        if (!collisionInfo.hasCollision) continue;

        if (collisionInfo.normal.y < -groundNormalThreshold)
        {
            totalNormalImpulse += std::abs(collisionInfo.normalImpulse);
            avgTangent += collisionInfo.tangent;
            ++groundContactCount;
        }
    }

    if (groundContactCount == 0) return;

    avgTangent /= static_cast<float>(groundContactCount);
    const float tangentLen = Helpers::LengthFast(avgTangent);
    if (tangentLen <= std::numeric_limits<float>::epsilon()) return;
    avgTangent /= tangentLen;

    const float tangentialVelocity = Helpers::DotProduct(prediction.vel, avgTangent);
    if (std::abs(tangentialVelocity) < Helpers::EPSILON) return;

    // Coulomb friction: maximum friction impulse = mu * totalNormalImpulse
    const float mu = prediction.collider->GetStaticFriction();
    const float maxFrictionImpulse = mu * totalNormalImpulse;
    const float neededImpulse = std::abs(tangentialVelocity) * mass;
    const float appliedImpulse = std::min(maxFrictionImpulse, neededImpulse);
    const float sign = (tangentialVelocity > 0.0f) ? -1.0f : 1.0f;
    const float deltaV = (appliedImpulse / mass) * sign;
    prediction.vel += avgTangent * deltaV;
}

void diji::PhysicsWorld::ApplyFriction(Prediction& prediction, const CollisionInfo& collision) const
{
    // if (collision.normalImpulse <= 0) return;
    
    const float tangentialVelocity = Helpers::DotProduct(prediction.vel, collision.tangent);
    if (std::abs(tangentialVelocity) < 0.001f) return;

    const float mass = prediction.collider->GetMass();
    
    // // Calculate friction coefficients
    // constexpr float staticFriction = 0.7f;   // Prevents sliding from starting
    // // Calculate maximum friction force (Coulomb friction model)
    // const float maxFrictionForce = staticFriction * collision.normalImpulse;
    // // Calculate required force to stop sliding completely
    // const float stoppingForce = std::abs(tangentialVelocity) * m_Mass / m_TimeSingletonInstance.GetFixedUpdateDeltaTime();
    // sf::Vector2f frictionForce;
    // if (stoppingForce <= maxFrictionForce)
    // {
    //     // Static friction: stop sliding completely
    //     frictionForce = -tangentialVelocity * collision.tangent * m_Mass / m_TimeSingletonInstance.GetFixedUpdateDeltaTime();
    // }
    // else
    // {
    //     constexpr float kineticFriction = 0.5f;
    //     // Kinetic friction: reduce sliding velocity
    //     const float kineticFrictionForce = kineticFriction * collision.normalImpulse;
    //     frictionForce = -std::copysign(kineticFrictionForce, tangentialVelocity) * collision.tangent;
    // }


    // Use normal force (mass * gravity) for friction
    const float gravityMagnitude = std::abs(m_Gravity.y);
    const float normalForce = mass * gravityMagnitude;
    
    const float kineticFriction = prediction.collider->GetStaticFriction();
    const float frictionMagnitude = kineticFriction * normalForce;
    sf::Vector2f frictionForce = -std::copysign(frictionMagnitude, tangentialVelocity) * collision.tangent;
    
    // Clamp friction so it doesn't reverse velocity
    const float maxFriction = std::abs(tangentialVelocity * mass / m_TimeSingletonInstance.GetFixedUpdateDeltaTime());
    if (frictionMagnitude > maxFriction)
        frictionForce = -tangentialVelocity * collision.tangent * mass / m_TimeSingletonInstance.GetFixedUpdateDeltaTime();
    
    // Apply friction to velocity (F = ma, so a = F/m, v += a*dt)
    prediction.vel += (frictionForce / mass) * m_TimeSingletonInstance.GetFixedUpdateDeltaTime();
}

void diji::PhysicsWorld::UpdateFinalPosition(const Prediction& prediction)
{
    prediction.collider->SetVelocity(prediction.vel);
    prediction.collider->GetParent()->SetObjectPosition(prediction.pos);
    prediction.collider->ClearNetForce();
}

diji::PhysicsWorld::CollisionDetectionResult diji::PhysicsWorld::HandleStaticCollisions(Prediction& dynamicCollider, const Collider* staticCollider)
{
    static CollisionDispatcher dispatcher;
    Prediction emptyStaticPrediction;
    return dispatcher.Dispatch(dynamicCollider, emptyStaticPrediction,dynamicCollider.collider, staticCollider);
}

diji::PhysicsWorld::CollisionDetectionResult diji::PhysicsWorld::HandleDynamicCollisions(Prediction& dynamicColliderA, Prediction& dynamicColliderB)
{
    static CollisionDispatcher dispatcher;
    return dispatcher.Dispatch(dynamicColliderA, dynamicColliderB, dynamicColliderA.collider, dynamicColliderB.collider);
}

void diji::PhysicsWorld::UpdateWorldBounds(const sf::FloatRect& aabb)
{
    if (m_DynamicColliders.empty() && m_StaticInfos.empty())
    {
        m_WorldBounds = aabb;
        return;
    }

    const float left   = std::min(m_WorldBounds.position.x, aabb.position.x);
    const float top    = std::min(m_WorldBounds.position.y, aabb.position.y);
    const float right  = std::max(m_WorldBounds.position.x + m_WorldBounds.size.x, aabb.position.x + aabb.size.x);
    const float bottom = std::max(m_WorldBounds.position.y + m_WorldBounds.size.y, aabb.position.y + aabb.size.y);

    m_WorldBounds.position.x   = left;
    m_WorldBounds.position.y    = top;
    m_WorldBounds.size.x  = right - left;
    m_WorldBounds.size.y = bottom - top;

    if (!m_QuadTree)
        m_QuadTree = std::make_unique<QuadTree>(m_WorldBounds);
    else
        m_QuadTree->SetWorldBounds(m_WorldBounds);
}