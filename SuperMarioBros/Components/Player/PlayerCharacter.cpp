#include "PlayerCharacter.h"

#include <array>

#include "PlayerInformation.h"
#include "../../Core/GameState.h"
#include "../../Helpers/MarioHelpers.h"
#include "../../Interfaces/IKillable.h"
#include "../../Interfaces/IShoveable.h"
#include "../PowerUps/FireBall.h"
#include "Engine/Singleton/SceneManager.h"
#include "Engine/Collision/Collider.h"
#include "../../Singletons/GameManager.h"
#include "../Enemies/KoopaTroopa.h"
#include "../Other/LevelObjects/Flag.h"
#include "../Other/HUD/PointsBehaviour.h"
#include "Engine/Components/Camera.h"
#include "Engine/Components/SpriteRenderComp.h"
#include "Engine/Components/TextComp.h"
#include "Engine/Components/Render.h"
#include "Engine/Components/Transform.h"
#include "Engine/Interfaces/IInterface.h"
#include "Engine/Interfaces/ISoundSystem.h"
#include "Engine/Singleton/GameStateManager.h"
#include "Engine/Singleton/Helpers.h"
#include "Engine/Singleton/RandNumber.h"
#include "Engine/Singleton/ResourceManager.h"
#include "Engine/Singleton/TimerManager.h"

void superMarioBros::PlayerCharacter::Init()
{
    m_CurrentStateUPtr = std::make_unique<IdleState>();
    m_CurrentStateUPtr->OnEnter(GetOwner());

    m_TransformCompPtr = GetOwner()->GetRootComponent();
    m_ColliderCompPtr = GetOwner()->GetComponent<diji::Collider>();
    m_SpriteRenderCompPtr = GetOwner()->GetComponent<diji::SpriteRenderComponent>();

    if (const auto camera = diji::SceneManager::GetInstance().GetMainCamera())
        camera->GetComponent<diji::Camera>()->SetFollow(GetOwner());
    // todo introduce error logging system  like  unity debug.logerror

    SetRenderLayer(1);
}

void superMarioBros::PlayerCharacter::Start()
{
    for(const auto enemyCollider : GameManager::GetInstance().GetEnemyColliders())
        m_ColliderCompPtr->OverlapCollider(enemyCollider);

    LoadPosition();
    
    sf::Shader& starShader = diji::ResourceManager::GetInstance().LoadShader("", "shaders/star.frag");
    m_SpriteRenderCompPtr->SetShader(&starShader);

    if (const auto flag = diji::SceneManager::GetInstance().GetGameObject("E_flag"))
        flag->GetComponent<Flag>()->OnFlagAnimationFinishedEvent.AddListener(this, &PlayerCharacter::StopFlagAnimAndMoveToCastle);

    CheckForSavedState();

    GameManager::GetInstance().OnBonusAreaTriggeredEvent.AddListener(this, &PlayerCharacter::PlayPipeTransitionAnimation);
}

#include <SFML/Window/Keyboard.hpp>
void superMarioBros::PlayerCharacter::Update()
{
    // temp
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::T))
        m_TransformCompPtr->SetWorldPosition(sf::Vector2f{ 9000, 200 });

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::R))
    {
        GameManager::GetInstance().AddLife();
        PlayDeathSequence();
    }
    
    if (m_IsDead || m_IsPaused) return;

    if (m_IsInvincible)
    {
        InvisibilityFlash();
        CheckEnemyStomp();
    }
    
    if (m_TransformCompPtr->GetWorldPosition().y > 600.f)
    {
        OnFallingInHoleEvent.Broadcast();
        HandleDeathSequence();
    }
}

void superMarioBros::PlayerCharacter::LateUpdate()
{
    if (m_IsDead || m_IsPaused) return;
    if (!m_IsStartPoweredUp) return;
    
    UpdateStarPowerShader();
}

void superMarioBros::PlayerCharacter::OnTriggerEnter(const diji::Collider* other, const diji::CollisionInfo&)
{
    if (m_IsDead || m_IsPaused) return;
    const std::string& otherTag = other->GetTag();

    if (otherTag == "flagPole")
        HandleLevelCompletion(other->GetPosition());

    if (otherTag != "enemy" && otherTag != "koopa" && otherTag != "plant") return;
    if (m_IsStartPoweredUp)
    {
        const auto enemyInterface = diji::InterfaceRegistry::GetInterface<IKillable>(other->GetParent());
        enemyInterface->Kill(m_TransformCompPtr->GetWorldPosition().x > other->GetPosition().x);
        return;
    }

    if (m_IsInvincible) return;
    if (otherTag == "plant")
    {
        HitByEnemy();
        return;
    }

    const bool isKoopa = otherTag == "koopa";
    if (isKoopa && other->GetParent()->GetComponent<KoopaTroopa>()->IsStomped())
    {
        const auto enemyInterface = diji::InterfaceRegistry::GetInterface<IShoveable>(other->GetParent());
        enemyInterface->Shove(m_TransformCompPtr->GetWorldPosition().x > other->GetPosition().x);
        
        const std::string& pointsString = MarioHelpers::GetStompPointsAsString(m_BounceScoreMultiplier + 3);
        OnEnemyStompedEvent.Broadcast(other, pointsString);
        return;
    }
    
    if (IsValidStomp(other))
    {
        diji::ServiceLocator::GetSoundSystem().AddSoundRequest("sound/smb_stomp.wav", false);

        if (isKoopa)
            StompKoopa(other);
        else
            StompEnemy(other);
    }
    else
    {
        if (isKoopa)
        {
            const auto enemyInterface = diji::InterfaceRegistry::GetInterface<IShoveable>(other->GetParent());
            enemyInterface->Shove(m_TransformCompPtr->GetWorldPosition().x > other->GetPosition().x);
        }
        HitByEnemy();
    }
}

bool superMarioBros::PlayerCharacter::IsValidStomp(const diji::Collider* other) const
{
    const bool isAbove = m_TransformCompPtr->GetWorldPosition().y <= other->GetPosition().y;

    bool isValid = m_ColliderCompPtr->GetVelocity().y > 0.f;
    if (!isValid)
        isValid = other->GetVelocity().y < 0.0f;

    return isAbove && isValid;
}

void superMarioBros::PlayerCharacter::OnHitEvent(const diji::Collider* other, const diji::CollisionInfo&)
{
    const std::string& otherTag = other->GetTag();

    if (!GROUND_TAGS.contains(otherTag))
        return;
    
    m_KoopaStompToggle = false;
    m_BounceScoreMultiplier = 0;
}

void superMarioBros::PlayerCharacter::SetTransitionState()
{
    m_PowerUpState = static_cast<PowerUpState>(GameManager::GetInstance().GetLastPlayerState());
    std::unique_ptr<PlayerStates> newState;
    if (m_PowerUpState == PowerUpState::Small)
        newState = std::make_unique<RunningState>();
    else if (m_PowerUpState == PowerUpState::Fire)
        newState = std::make_unique<FireRunningState>();
    else
        newState = std::make_unique<BigRunningState>();
    m_CurrentStateUPtr = std::move(newState);
    m_CurrentStateUPtr->OnEnter(GetOwner());
}

void superMarioBros::PlayerCharacter::OnPowerUpCollected(const PowerUpType power)
{
    switch (power)
    {
    case PowerUpType::Mushroom:
    case PowerUpType::FireFlower:
        HandlePowerUpCollision();
        break;
    case PowerUpType::OneUpMushroom:
        GameManager::GetInstance().AddLife();
        break;
    case PowerUpType::Star:
        HandleStarPickup();
        break;
    case PowerUpType::None:
    default:  // NOLINT(clang-diagnostic-covered-switch-default)
        break;
    }
}

void superMarioBros::PlayerCharacter::HandleDeathSequence()
{
    diji::ServiceLocator::GetSoundSystem().AddSoundRequest("sound/smb_mariodie.wav", false);
    diji::ServiceLocator::GetSoundSystem().StopMusic();

    m_IsDead = true;

    auto newState = std::make_unique<DeathState>();
    m_CurrentStateUPtr = std::move(newState);
    m_CurrentStateUPtr->OnEnter(GetOwner());
    m_ColliderCompPtr->SetVelocity(sf::Vector2f{ 0, 0 });
    m_ColliderCompPtr->SetAffectedByGravity(false);

    (void)diji::TimerManager::GetInstance().SetTimer(this, [&]()
    {
        PlayDeathSequence();
    }, 0.25f, false);
}

void superMarioBros::PlayerCharacter::PlayDeathSequence() const
{
    m_ColliderCompPtr->SetAffectedByGravity(true);
    m_ColliderCompPtr->SetCollisionResponse(diji::Collider::CollisionResponse::Overlap);

    m_ColliderCompPtr->ApplyImpulse(sf::Vector2f{ 0, -DEATH_BUMP_STRENGTH });

    (void)diji::TimerManager::GetInstance().SetTimer(this, [&]()
    {
        GameManager::GetInstance().LoseLife();
        GameManager::GetInstance().ResetLevel();
        
    }, 3.41f, false);
}

void superMarioBros::PlayerCharacter::LoadPosition() const
{
    if ( static_cast<superMarioBrosState>(diji::GameStateManager::GetInstance().GetCurrentGameState()) != superMarioBrosState::Level) return;
    
    const auto& gameManager = GameManager::GetInstance();
    if (gameManager.IsCheckPointActivated())
        m_TransformCompPtr->SetWorldPosition(gameManager.GetCheckPointPosition());
    else
        m_TransformCompPtr->SetWorldPosition(static_cast<sf::Vector2f>(gameManager.GetStartPosition()));
}

void superMarioBros::PlayerCharacter::PlayGrowthAnimation()
{
    m_IsPaused = true;
    
    // play animation
    std::unique_ptr<PlayerStates> newState = std::make_unique<GrowthAnimationState>();
    m_CurrentStateUPtr = std::move(newState);
    m_CurrentStateUPtr->OnEnter(GetOwner());

    m_ColliderCompPtr->ResizeCollider(sf::Vector2f{ 48, 96 });
    m_TransformCompPtr->SetWorldPosition(m_TransformCompPtr->GetWorldPosition() + sf::Vector2f{ 0.f, -24.f });
    
    (void)diji::TimerManager::GetInstance().SetTimer(this, [&]()
    {
        std::unique_ptr<PlayerStates> newBigState = std::make_unique<BigIdleState>();
        m_CurrentStateUPtr = std::move(newBigState);
        m_CurrentStateUPtr->OnEnter(GetOwner());
    
        m_IsPaused = false;
        OnPoweringUpEvent.Broadcast(false);
        m_ColliderCompPtr->SetAffectedByGravity(true);
    }, 0.78f, false);
}

void superMarioBros::PlayerCharacter::PlayShrinkAnimation()
{
    diji::ServiceLocator::GetSoundSystem().AddSoundRequest("sound/smb_pipe.wav", false);

    m_IsPaused = true;
    OnPoweringUpEvent.Broadcast(true);
    GameManager::GetInstance().SwitchCurrentPlayerState();

    // play animation
    std::unique_ptr<PlayerStates> newState = std::make_unique<ShrinkAnimationState>();
    m_CurrentStateUPtr = std::move(newState);
    m_CurrentStateUPtr->OnEnter(GetOwner());
    m_ColliderCompPtr->SetAffectedByGravity(false);
    m_ColliderCompPtr->SetVelocity(sf::Vector2f{ 0.f, 0.f });
    
    (void)diji::TimerManager::GetInstance().SetTimer(this, [&]()
    {
        m_SpriteRenderCompPtr->SetStartingFrameX(2);
        m_SpriteRenderCompPtr->SetTotalAnimationFrames(8);
        m_SpriteRenderCompPtr->SetCurrentAnimationFrame(0);
        m_SpriteRenderCompPtr->UpdateFrame();
    }, 0.32f, false);

    (void)diji::TimerManager::GetInstance().SetTimer(this, [&]()
    {
        m_ColliderCompPtr->SetAffectedByGravity(true);
        m_ColliderCompPtr->ResizeCollider(sf::Vector2f{ 48, 48 });

        std::unique_ptr<PlayerStates> newBigState = std::make_unique<IdleState>();
        m_CurrentStateUPtr = std::move(newBigState);
        m_CurrentStateUPtr->OnEnter(GetOwner());

        m_IsInvincible = true;
        m_InvincibilityTimer = 2.2f;
        m_IsPaused = false;
        OnPoweringUpEvent.Broadcast(false);
    }, 1.12f, false);
}

void superMarioBros::PlayerCharacter::HandlePowerUpCollision()
{
    diji::ServiceLocator::GetSoundSystem().AddSoundRequest("sound/smb_powerup.wav", false);

    bool skip = false;
    m_ColliderCompPtr->SetAffectedByGravity(false);
    if (m_PowerUpState == PowerUpState::Small)
    {
        m_ColliderCompPtr->SetVelocity(sf::Vector2f{ 0.f, 0.f });
        GameManager::GetInstance().SwitchCurrentPlayerState();
        m_PowerUpState = PowerUpState::Big;
        PlayGrowthAnimation();
    }
    else if (m_PowerUpState == PowerUpState::Big)
    {
        m_ColliderCompPtr->SetVelocity(sf::Vector2f{ 0.f, 0.f });
        m_PowerUpState = PowerUpState::Fire;
        PlayFireTransitionAnimation();
    }
    else
    {
        skip = true;
        m_ColliderCompPtr->SetAffectedByGravity(true);
    }

    GameManager::GetInstance().OnScoreAddedEvent.Broadcast(1000);
    if (skip) return;
    OnPoweringUpEvent.Broadcast(true);
}

void superMarioBros::PlayerCharacter::InvisibilityFlash()
{
    m_InvincibilityTimer -= m_TimeSingletonInstance.GetDeltaTime();
    m_InvincibilityRenderTimer -= m_TimeSingletonInstance.GetDeltaTime();

    if (m_InvincibilityRenderTimer <= 0.f)
    {
        m_InvincibilityRenderTimer += 0.05f;
        m_SpriteRenderCompPtr->ToggleRendering();
    }
    
    if (m_InvincibilityTimer <= 0.f)
    {
        m_InvincibilityRenderTimer = 0.f;
        m_SpriteRenderCompPtr->EnableRender();
        m_IsInvincible = false;
    }
}

void superMarioBros::PlayerCharacter::CheckEnemyStomp()
{
    const sf::Vector2f origin = m_ColliderCompPtr->GetPosition();
    constexpr sf::Vector2f dir = { 0, 1 };
    constexpr float offset = 23.f;
    const sf::Vector2f TopLeft = { origin.x - offset, origin.y + offset };
    const sf::Vector2f TopRight = { origin.x + offset, origin.y + offset };
    
    auto ValidateEnemyStomp = [&](const diji::Collider* other) -> void
    {
        if (!IsValidStomp(other)) return;

        if (other->GetTag() == "koopa")
            StompKoopa(other);
        else
            StompEnemy(other);
    };
    
    if (const auto hit =  diji::SceneManager::GetInstance().GetPhysicsWorld()->Raycast(TopLeft, dir, 10.f, m_ColliderCompPtr))
    {
        if (hit->info.hasCollision && hit->collider->GetTag() == "enemy")
            ValidateEnemyStomp(hit->collider);
    }

    if (const auto hit =  diji::SceneManager::GetInstance().GetPhysicsWorld()->Raycast(TopRight, dir, 10.f, m_ColliderCompPtr))
    {
        if (hit->info.hasCollision && hit->collider->GetTag() == "enemy")
            ValidateEnemyStomp(hit->collider);
    }
}

void superMarioBros::PlayerCharacter::StompEnemy(const diji::Collider* other)
{
    // I'm capping vertical velocity so max it out to ensure the bounce is same height as normal jump
    m_ColliderCompPtr->SetVelocity(sf::Vector2f{ m_ColliderCompPtr->GetVelocity().x, 0.f });
    m_ColliderCompPtr->ApplyImpulse(sf::Vector2f(0, -DEATH_BUMP_STRENGTH));
    
    ++m_BounceScoreMultiplier;
    const std::string& pointsString = MarioHelpers::GetStompPointsAsString(m_BounceScoreMultiplier);
    OnEnemyStompedEvent.Broadcast(other, pointsString);
    
    m_ColliderCompPtr->IgnoreCollider(other);
}

void superMarioBros::PlayerCharacter::StompKoopa(const diji::Collider* other)
{
    m_ColliderCompPtr->SetVelocity(sf::Vector2f{ m_ColliderCompPtr->GetVelocity().x, 0.f });
    m_ColliderCompPtr->ApplyImpulse(sf::Vector2f(0, -DEATH_BUMP_STRENGTH));

    m_KoopaStompToggle = !m_KoopaStompToggle;

    if (m_KoopaStompToggle)
        ++m_BounceScoreMultiplier;

    const std::string& pointsString = m_KoopaStompToggle ? MarioHelpers::GetStompPointsAsString(m_BounceScoreMultiplier) : "";
    OnEnemyStompedEvent.Broadcast(other, pointsString);
}

void superMarioBros::PlayerCharacter::PlayFireTransitionAnimation()
{
    m_IsPaused = true;
    
    // play animation
    std::unique_ptr<PlayerStates> newState = std::make_unique<FireAnimationState>();
    m_CurrentStateUPtr = std::move(newState);
    m_CurrentStateUPtr->OnEnter(GetOwner());

    (void)diji::TimerManager::GetInstance().SetTimer(this, [&]()
    {
        std::unique_ptr<PlayerStates> newBigState = std::make_unique<FireIdleState>();
        m_CurrentStateUPtr = std::move(newBigState);
        m_CurrentStateUPtr->OnEnter(GetOwner());
    
        m_IsPaused = false;
        OnPoweringUpEvent.Broadcast(false);
        m_ColliderCompPtr->SetAffectedByGravity(true);
    }, 0.78f, false);
}

void superMarioBros::PlayerCharacter::PlayPipeTransitionAnimation(const diji::Directions pipeEntranceDirection)
{
    sf::Vector2f direction;

    // todo: introduce a converter in utils.
    switch (pipeEntranceDirection)
    {
    case diji::Directions::Left:
        direction = { -1.0f, 0.0f };
        break;

    case diji::Directions::Right:
        direction = { 1.0f, 0.0f };
        break;

    case diji::Directions::Up:
        direction = { 0.0f, -1.0f };
        break;

    case diji::Directions::Down:
        direction = { 0.0f, 1.0f };
        break;
    case diji::Directions::None:
        // todo: throw an error
        break;
    }

    m_IsPaused = true;
    OnPipeTransitionEvent.Broadcast(true);
    diji::ServiceLocator::GetSoundSystem().AddSoundRequest("sound/smb_pipe.wav", false);

    m_PreviousCollisionResponse = m_ColliderCompPtr->GetCollisionResponse();
    m_ColliderCompPtr->SetCollisionResponse(diji::Collider::CollisionResponse::Ignore);
    m_ColliderCompPtr->SetAffectedByGravity(false);
    m_ColliderCompPtr->SetVelocity(sf::Vector2f{ 0.0f, 0.0f });

    const float playerSize = direction.y != 0.0f ? GetPlayerHeight() : PLAYER_WIDTH;

    MoveThroughPipe(direction, playerSize * PIPE_TRAVEL_DISTANCE_MULTIPLIER, playerSize, false, [this]()
    {
        GameManager::GetInstance().HideCurrentArea();

        (void)diji::TimerManager::GetInstance().SetTimer(this, [this]()
        {
            auto& gameManager = GameManager::GetInstance();
            const AreaArrival nextArea = gameManager.ShowNextArea();

            if (nextArea.exitType == WarpExitType::Drop)
            {
                TeleportPlayer(nextArea.position);
                EndPipeTransition();
                return;
            }

            const float height = GetPlayerHeight();
            const float distance = height * PIPE_TRAVEL_DISTANCE_MULTIPLIER;
            TeleportPlayer(sf::Vector2f{ nextArea.position.x, nextArea.position.y - height * 0.5f + distance });
            diji::ServiceLocator::GetSoundSystem().AddSoundRequest("sound/smb_pipe.wav", false);

            MoveThroughPipe(UP_VECTOR, distance, height, true, [this]()
            {
                EndPipeTransition();
            });
        }, PIPE_BLACK_SCREEN_DURATION, false);
    });
}

void superMarioBros::PlayerCharacter::MoveThroughPipe(const sf::Vector2f& direction, const float distance, const float playerSize, const bool isExitingPipe, const std::function<void()>& onFinished)
{
    const sf::Vector2f originalPos = m_TransformCompPtr->GetWorldPosition();
    UpdatePipeVisibility(isExitingPipe ? distance : 0.0f, playerSize);

    m_PipeTimelinePtr = diji::SceneManager::GetInstance().CreateTimeline(GetOwner());
    auto& track = m_PipeTimelinePtr->AddFloatTrack("MoveThroughPipe");

    track.keys =
    {
        { .time = 0.0f,                     .value = 0.0f },
        { .time = PIPE_TRANSITION_DURATION, .value = distance }
    };

    track.onValue = [this, originalPos, direction, distance, playerSize, isExitingPipe](const float currentDistance)
    {
        m_TransformCompPtr->SetWorldPosition(
        {
            originalPos.x + direction.x * currentDistance,
            originalPos.y + direction.y * currentDistance
        });

        UpdatePipeVisibility(isExitingPipe ? distance - currentDistance : currentDistance, playerSize);
    };

    auto& [eventName, eventKeysVec] = m_PipeTimelinePtr->GetEventTrack("OnAnimationEnd");
    eventKeysVec =
    {
        { .time= PIPE_TRANSITION_DURATION, .callback= onFinished }
    };
}

void superMarioBros::PlayerCharacter::EndPipeTransition()
{
    m_ColliderCompPtr->SetCollisionResponse(m_PreviousCollisionResponse);
    m_ColliderCompPtr->SetAffectedByGravity(true);
    m_ColliderCompPtr->SetVelocity(sf::Vector2f{ 0.0f, 0.0f });
    m_SpriteRenderCompPtr->EnableRender();

    m_IsPaused = false;
    OnPipeTransitionEvent.Broadcast(false);
}

void superMarioBros::PlayerCharacter::UpdatePipeVisibility(const float depthInPipe, const float playerSize) const
{
    if (depthInPipe < playerSize)
        m_SpriteRenderCompPtr->EnableRender();
    else
        m_SpriteRenderCompPtr->DisableRender();
}

void superMarioBros::PlayerCharacter::TeleportPlayer(const sf::Vector2f& position) const
{
    m_TransformCompPtr->SetWorldPosition(position);

    // syncing twice sets both the last and new collider positions, otherwise the collider interpolates from the old position for a frame
    m_ColliderCompPtr->SyncTransform();
    m_ColliderCompPtr->SyncTransform();
}

float superMarioBros::PlayerCharacter::GetPlayerHeight() const
{
    return m_PowerUpState == PowerUpState::Small ? PLAYER_WIDTH : PLAYER_WIDTH * 2.0f;
}

void superMarioBros::PlayerCharacter::HandleStarPickup()
{
    diji::ServiceLocator::GetSoundSystem().AddSoundRequest("sound/smb_powerup.wav", false);
    diji::ServiceLocator::GetSoundSystem().AddSoundRequest("sound/InvincibilityNew.mp3", true);

    m_IsStartPoweredUp = true;
    m_StarPowerTimer = 0;
    m_SpriteRenderCompPtr->SetRenderWithShader(true);
}

void superMarioBros::PlayerCharacter::UpdateStarPowerShader()
{
    m_StarPowerTimer += m_TimeSingletonInstance.GetDeltaTime();
    const auto starShader = m_SpriteRenderCompPtr->GetShader();
    
    starShader->setUniform("texture", sf::Shader::CurrentTexture);
    starShader->setUniform("time", m_StarPowerTimer);
    constexpr float flashSpeed = 16.0f;  // 8 cycles per second
    const int paletteIndex = static_cast<int>(m_StarPowerTimer * flashSpeed) % 4;
    starShader->setUniform("paletteIndex", paletteIndex);

    if (m_StarPowerTimer >= 11.f)
    {
        m_IsStartPoweredUp = false;
        m_SpriteRenderCompPtr->SetRenderWithShader(false);
        diji::ServiceLocator::GetSoundSystem().AddSoundRequest("sound/LevelMusic.mp3", true);
    }
}

void superMarioBros::PlayerCharacter::HandleLevelCompletion(const sf::Vector2f& center)
{
    diji::ServiceLocator::GetSoundSystem().AddSoundRequest("sound/smb_flagpole.wav", false);
    diji::ServiceLocator::GetSoundSystem().StopMusic();

    m_IsPaused = true;
    m_ColliderCompPtr->SetVelocity(sf::Vector2f{ 0, 0});
    m_ColliderCompPtr->SetAffectedByGravity(false);

    m_FlagCenter = center;
    m_TransformCompPtr->SetWorldPosition(sf::Vector2f{ center.x - 20.f, m_TransformCompPtr->GetWorldPosition().y });
    m_SpriteRenderCompPtr->SetSpriteLookingRight();
    std::unique_ptr<PlayerStates> newState;
    if (m_PowerUpState == PowerUpState::Small)
        newState = std::make_unique<FlagPoleSlideState>();
    else if (m_PowerUpState == PowerUpState::Fire)
        newState = std::make_unique<FireFlagPoleSlideState>();
    else
        newState = std::make_unique<BigFlagPoleSlideState>();
    
    m_CurrentStateUPtr = std::move(newState);
    m_CurrentStateUPtr->OnEnter(GetOwner());

    // create timeline for moving down the pole
    m_FlagPoleTimelinePtr = diji::SceneManager::GetInstance().CreateTimeline(GetOwner());
    sf::Vector2f originalPos = m_TransformCompPtr->GetWorldPosition();

    const float groundDistance = m_PowerUpState == PowerUpState::Small ? 476.0f : 452.0f;
    const float distanceToMove = groundDistance - originalPos.y;
    constexpr float moveDuration = 2.f/15.f / 50.f; // 8frames to move 50 units
    auto &track = m_FlagPoleTimelinePtr->AddFloatTrack("MoveVertically");
    track.keys = { { .time= 0.f, .value= 0.f }, { .time= distanceToMove * moveDuration, .value= distanceToMove } };
    
    track.onValue = [&, originalPos](const float y)
    {
        m_TransformCompPtr->SetWorldPosition(sf::Vector2f{ originalPos.x, originalPos.y + y });
    };

    // handle pole score
    const int flagPoleChunk = static_cast<int>(std::floorf(distanceToMove * 0.01f));
    constexpr std::array flagPointTable = { 100, 400, 800, 2000, 5000 };
    const int index = std::clamp(flagPoleChunk, 0, static_cast<int>(flagPointTable.size() - 1));
    const int flagPoints = flagPointTable[index];

    GameManager::GetInstance().OnScoreAddedEvent.Broadcast(flagPoints);
    const std::string pointsString = std::to_string(flagPoints);
    auto pointsText = std::make_unique<diji::GameObject>();
    pointsText->SetObjectPosition(sf::Vector2f{ originalPos.x + 65.f, 425.f });
    pointsText->AddComponent<diji::TextComp>(pointsString, "fonts/PressStart2P-vaV7.ttf", sf::Color::White, true);
    pointsText->GetComponent<diji::TextComp>()->GetText().setCharacterSize(18);
    pointsText->AddComponent<diji::Render>();
    pointsText->AddComponent<PointsBehaviour>(true);
    pointsText->GetComponent<PointsBehaviour>()->SetSpeed(-375.f);
    pointsText->GetComponent<PointsBehaviour>()->SetMaxHeight(-75.f);

    diji::SceneManager::GetInstance().SpawnGameObject("ZZ_pointsText", std::move(pointsText), sf::Vector2f{ originalPos.x + 65.f, 425.f });
    
    OnLevelFinishedEvent.Broadcast();
}

void superMarioBros::PlayerCharacter::StopFlagAnimAndMoveToCastle()
{
    if (m_IsFlagTriggered) return;

    m_IsFlagTriggered = true;
    m_FlagPoleTimelinePtr->Stop();
    m_SpriteRenderCompPtr->SetSpriteLookingLeft();
    m_SpriteRenderCompPtr->Pause();
    TeleportPlayer(sf::Vector2f{ m_FlagCenter.x + 20.0f, m_TransformCompPtr->GetWorldPosition().y });
    diji::ServiceLocator::GetSoundSystem().AddSoundRequest("sound/smb_stage_clear.wav", false);

    (void)diji::TimerManager::GetInstance().SetTimer(this, [&]
    {
        m_SpriteRenderCompPtr->SetSpriteLookingRight();
        m_ColliderCompPtr->SetAffectedByGravity(true);
        std::unique_ptr<PlayerStates> newState;
        if (m_PowerUpState == PowerUpState::Small)
            newState = std::make_unique<WalkingState>();
        else if (m_PowerUpState == PowerUpState::Fire)
            newState = std::make_unique<FireWalkingState>();
        else
            newState = std::make_unique<BigWalkingState>();
        m_CurrentStateUPtr = std::move(newState);
        m_CurrentStateUPtr->OnEnter(GetOwner());

        // create timeline for moving down the pole
        m_FlagPoleTimelinePtr = diji::SceneManager::GetInstance().CreateTimeline(GetOwner());
        sf::Vector2f originalPos = m_TransformCompPtr->GetWorldPosition();

        auto &track = m_FlagPoleTimelinePtr->AddFloatTrack("MoveVertically");
        track.keys = { { .time= 0.f, .value= 0.f }, { .time= 1.25f, .value= 700 } };
            
        track.onValue = [&, originalPos](const float x)
        {
            m_TransformCompPtr->SetWorldPosition(sf::Vector2f{ originalPos.x + x, m_TransformCompPtr->GetWorldPosition().y });
        };

        auto& [eventName, eventKeysVec] = m_FlagPoleTimelinePtr->GetEventTrack("OnAnimationEnd");
        eventKeysVec =
        {
            { .time= 1.25f, .callback= [&]()
                {
                    OnCastleReachedEvent.Broadcast();
                    SetActive(false);
                }
            }
        };
    }, 0.4f, false);
}

void superMarioBros::PlayerCharacter::CheckForSavedState()
{
    m_PowerUpState = static_cast<PowerUpState>(GameManager::GetInstance().GetLastPlayerState());

    if (m_PowerUpState != PowerUpState::Small)
    {
        m_ColliderCompPtr->ResizeCollider(sf::Vector2f{ 48, 96 });
        m_TransformCompPtr->SetWorldPosition(m_TransformCompPtr->GetWorldPosition() + sf::Vector2f{ 0.f, -24.f });

        std::unique_ptr<PlayerStates> newState;
        if (m_PowerUpState == PowerUpState::Fire)
            newState = std::make_unique<FireIdleState>();
        else
            newState = std::make_unique<BigIdleState>();
        
        m_CurrentStateUPtr = std::move(newState);
        m_CurrentStateUPtr->OnEnter(GetOwner());
    }
}

void superMarioBros::PlayerCharacter::HitByEnemy()
{
    OnHitByEnemyEvent.Broadcast();
    if (m_PowerUpState == PowerUpState::Big || m_PowerUpState == PowerUpState::Fire)
    {
        m_PowerUpState = PowerUpState::Small;
        PlayShrinkAnimation();
    }
    else
        HandleDeathSequence();
}
