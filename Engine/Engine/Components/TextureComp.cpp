#include "TextureComp.h"

#include "Render.h"
#include "../Singleton/ResourceManager.h"
#include "../Core/GameObject.h"

diji::TextureComp::TextureComp(GameObject* ownerPtr)
	: Component(ownerPtr)
{
}

diji::TextureComp::TextureComp(GameObject* ownerPtr, std::string filename)
	: Component(ownerPtr)
	, m_FilePath{std::move(filename)}
{
}

diji::TextureComp::TextureComp(GameObject* ownerPtr, std::string filename, const float scaleX, const float scaleY)
	: Component(ownerPtr)
	, m_FilePath{ std::move(filename) }
	, m_ScaleX{ scaleX }
	, m_ScaleY{ scaleY }
{
}

diji::TextureComp::TextureComp(GameObject* ownerPtr, std::string filename, const float scale)
	: Component(ownerPtr)
	, m_FilePath{ std::move(filename) }
	, m_ScaleX{ scale }
	, m_ScaleY{ scale }
{
}

void diji::TextureComp::Init()
{
	if (not m_FilePath.empty())
		SetTexture(m_FilePath);
	// else console log warning path not set or something
}

void diji::TextureComp::SetTexture(const std::string& filename)
{
	m_SFMLTexture = ResourceManager::GetInstance().LoadTexture(filename);
	if (GetOwner()->HasComponent<Render>())
		GetOwner()->GetComponent<Render>()->UpdateTexture(m_SFMLTexture);

	if (m_NeedsCentering)
		SetOriginToCenter();
}

void diji::TextureComp::SetScaleX(const float scaleX)
{
	m_ScaleX = scaleX;
}

void diji::TextureComp::SetScaleY(const float scaleY)
{
	m_ScaleY = scaleY;
}

void diji::TextureComp::SetScale(const float scale)
{
	m_ScaleX = scale;
	m_ScaleY = scale;
}

void diji::TextureComp::SetWidth(const int width)
{
	SetScaleX(static_cast<float>(width) / static_cast<float>(m_SFMLTexture.getSize().x));
}

void diji::TextureComp::SetHeight(const int height)
{
	SetScaleY(static_cast<float>(height) / static_cast<float>(m_SFMLTexture.getSize().y));
}

void diji::TextureComp::SetOriginToCenter()
{
	m_IsCentered = true;
	const auto textureSize = m_SFMLTexture.getSize();
	m_Origin = { static_cast<float>(textureSize.x) * 0.5f, static_cast<float>(textureSize.y) * 0.5f };

	// if texture is not set yet, dirty flag it
	if (textureSize.x == 0 || textureSize.y == 0)
		m_NeedsCentering = true;
	else if (m_NeedsCentering)
		m_NeedsCentering = false;
}

// todo: consider whether I should use float for sizes?
int diji::TextureComp::GetWidth() const
{
	return static_cast<int>(static_cast<float>(m_SFMLTexture.getSize().x) * m_ScaleX);
}

int diji::TextureComp::GetHeight() const
{
	return static_cast<int>(static_cast<float>(m_SFMLTexture.getSize().y) * m_ScaleY);
}

sf::Vector2u diji::TextureComp::GetSize() const
{
	return m_SFMLTexture.getSize();
}
