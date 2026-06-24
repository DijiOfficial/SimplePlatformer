#include "OnScreenKey.h"

#include "Engine/Components/TextComp.h"
#include "Engine/Core/GameObject.h"

void onScreenKeyboard::OnScreenKey::Init()
{
    m_TextCompPtr = GetOwner()->GetComponent<diji::TextComp>();
}

void onScreenKeyboard::OnScreenKey::SetKey(const char key)
{
    m_Key = key;
    m_TextCompPtr->GetText().setString(std::string(1, key)); // TODO handle special keys like space, backspace, shift etc.
}
