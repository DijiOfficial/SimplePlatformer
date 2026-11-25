#pragma once
#include "Component.h"
#include <string>
#include "../RAII_Wrappers/TextureHandle.h"

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
		[[nodiscard]] TextureHandle GetTexture() const { return m_TextureHandle; }

		void SetScaleX(float scaleX) const;
		void SetScaleY(float scaleY) const;
		void SetScale(float scale) const;
		void SetWidth(int width) const;
		void SetHeight(int height) const;
		void SetOrigin(const sf::Vector2f& origin) { m_Origin = origin; }
		void SetOrigin(float x, float y) { m_Origin = { x, y }; }
		void SetOriginToCenter();

		int GetWidth() const;
		int GetHeight() const;
		float GetScaleX() const;
		float GetScaleY() const;
		sf::Vector2f GetOrigin() const { return m_Origin; }
		sf::Vector2u GetSize() const;
		bool IsCentered() const { return m_IsCentered; }
		
	private:
		TextureHandle m_TextureHandle;
		std::string m_FilePath;
		sf::Vector2f m_Origin = { 0.0f, 0.0f };
		
		bool m_NeedsCentering = true;
		bool m_IsCentered = false;
	};
}

