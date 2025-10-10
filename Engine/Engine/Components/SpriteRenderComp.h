#pragma once
#include "Render.h"
#include "../Singleton/TimeSingleton.h"

#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Sprite.hpp>

namespace diji
{
    class SpriteRenderComponent final : public Render
    {
    public:
        explicit SpriteRenderComponent(GameObject* ownerPtr, const std::string& texturePath);
        explicit SpriteRenderComponent(GameObject* ownerPtr, const std::string& texturePath, const sf::Vector2i& frameSize, int totalAnimationFrames, float frameDurationSec);
        ~SpriteRenderComponent() noexcept override = default;
        
        void Init() override;
        void OnEnable() override {}
        void Start() override;
        
        void Update() override {}
        void FixedUpdate() override {}
        void LateUpdate() override;

        void OnDisable() override {}
        void OnDestroy() override {}

        void RenderFrame() const override;

        void SetFrameSize(const sf::Vector2i& size);
        void SetFrameSizeX(int x);
        void SetFrameSizeY(int y);

        void SetStartingFrameX(const int frame) { m_StartingFrameX = frame; }
        void SetStartingFrameY(const int frame) { m_StartingFrameY = frame; }
        void SetStartingFrame(const int x, const int y) { m_StartingFrameX = x; m_StartingFrameY = y; }

        void SetTotalAnimationFrames(const int count);
        void SetCurrentAnimationFrame(int frame);

        void SetScale(float scale) override;
        void InvertSprite();
        void SetSpriteLookingLeft();
        void SetSpriteLookingRight();

        void Play() { m_IsPlaying = true; }
        void Pause() { m_IsPlaying = false; }

        void SetLooping(const bool loop) { m_IsLooping = loop; }
        void SetFrameDuration(const float time) { m_FrameDuration = time; }
        void SetTotalAnimationTime(float time);

        void UpdateFrame();

    private:
        const TimeSingleton& m_TimeSingletonInstance = TimeSingleton::GetInstance();
        Transform* m_TransformCompPtr = nullptr;
        sf::Texture m_Texture;
        sf::Sprite m_Sprite;
        sf::Vector2i m_FrameSize;
        int m_CurrentFrame = 0;
        int m_StartingFrameX = 0;  
        int m_StartingFrameY = 0;
        int m_TotalAnimationFrames = 1;
        float m_AnimationTimer = 0.f;
        float m_FrameDuration = 0.25f;

        bool m_IsPlaying = true;
        bool m_IsLooping = true;
    };
}
