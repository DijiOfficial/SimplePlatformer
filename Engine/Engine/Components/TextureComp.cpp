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
{
	ownerPtr->SetObjectScale2D(sf::Vector2f{ scaleX, scaleY });
}

diji::TextureComp::TextureComp(GameObject* ownerPtr, std::string filename, const float scale)
	: Component(ownerPtr)
	, m_FilePath{ std::move(filename) }
{
	ownerPtr->SetObjectScale2D(sf::Vector2f{ scale, scale });
}

void diji::TextureComp::Init()
{
	if (not m_FilePath.empty())
		SetTexture(m_FilePath);
	// else log warning path not set or something
}

void diji::TextureComp::SetTexture(const std::string& filename)
{
	m_SFMLTexture = ResourceManager::GetInstance().LoadTexture(filename);
	if (GetOwner()->HasComponent<Render>())
		GetOwner()->GetComponent<Render>()->UpdateTexture(m_SFMLTexture);

	if (m_NeedsCentering)
		SetOriginToCenter();
}

void diji::TextureComp::SetScaleX(const float scaleX) const
{
	const auto* ownerPtr = GetOwner();
	ownerPtr->SetObjectScale2D(sf::Vector2f{ scaleX, ownerPtr->GetObjectScale2D().y });
}

void diji::TextureComp::SetScaleY(const float scaleY) const
{
	const auto* ownerPtr = GetOwner();
	ownerPtr->SetObjectScale2D(sf::Vector2f{ ownerPtr->GetObjectScale2D().x, scaleY });
}

void diji::TextureComp::SetScale(const float scale) const
{
	GetOwner()->SetObjectScale2D(sf::Vector2f{ scale, scale });
}

void diji::TextureComp::SetWidth(const int width) const
{
	SetScaleX(static_cast<float>(width) / static_cast<float>(m_SFMLTexture.getSize().x));
}

void diji::TextureComp::SetHeight(const int height) const
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
	return static_cast<int>(static_cast<float>(m_SFMLTexture.getSize().x) * GetOwner()->GetObjectScale2D().x);
}

int diji::TextureComp::GetHeight() const
{
	return static_cast<int>(static_cast<float>(m_SFMLTexture.getSize().y) * GetOwner()->GetObjectScale2D().y);
}

float diji::TextureComp::GetScaleX() const
{
	return GetOwner()->GetObjectScale2D().x;
}

float diji::TextureComp::GetScaleY() const
{
	return GetOwner()->GetObjectScale2D().y;
}

sf::Vector2u diji::TextureComp::GetSize() const
{
	return m_SFMLTexture.getSize();
}
