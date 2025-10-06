#pragma once
#include "Component.h"
#include <string>
#include <SFML/Graphics/Texture.hpp> // can I remove this?

namespace sf
{
	class Texture;
	class Angle;
}

namespace diji
{
	class GameObject;

	// todo if render is different from component then this should inherit from that
	class TextureComp final : public Component
	{
	public:
		explicit TextureComp(GameObject* ownerPtr);
		TextureComp(GameObject* ownerPtr, std::string filename);
		TextureComp(GameObject* ownerPtr, std::string filename, float scaleX, float scaleY);
		TextureComp(GameObject* ownerPtr, std::string filename, float scale);
		~TextureComp() noexcept override = default;

		TextureComp(const TextureComp& other) = delete;
		TextureComp(TextureComp&& other) = delete;
		TextureComp& operator=(const TextureComp& other) = delete;
		TextureComp& operator=(TextureComp&& other) = delete;

		void Init() override;
		void OnEnable() override {}
		void Start() override {}
		
		void FixedUpdate() override {}
		void Update() override {}
		void LateUpdate() override {}

		void OnDisable() override {}
		void OnDestroy() override {}

		void SetTexture(const std::string& filename);
		sf::Texture GetTexture() const { return m_SFMLTexture; }

		void SetScaleX(float scaleX);
		void SetScaleY(float scaleY);
		void SetScale(float scale);
		void SetWidth(int width);
		void SetHeight(int height);
		void SetRotationAngle(const float angle) { m_RotationAngle = angle; }
		void SetOrigin(const sf::Vector2f& origin) { m_Origin = origin; }
		void SetOrigin(float x, float y) { m_Origin = { x, y }; }
		void SetOriginToCenter();

		int GetWidth() const;
		int GetHeight() const;
		float GetScaleX() const { return m_ScaleX; }
		float GetScaleY() const { return m_ScaleY; }
		float GetRotationAngle() const { return m_RotationAngle; }
		sf::Vector2f GetOrigin() const { return m_Origin; }
		sf::Vector2u GetSize() const;
		bool IsCentered() const { return m_IsCentered; }
		
	private:
		sf::Texture m_SFMLTexture;
		std::string m_FilePath;
		sf::Vector2f m_Origin = { 0.f, 0.f };
		
		float m_RotationAngle = 0.f;
		float m_ScaleX = 1;
		float m_ScaleY = 1;

		bool m_NeedsCentering = true;
		bool m_IsCentered = false;
	};
}

