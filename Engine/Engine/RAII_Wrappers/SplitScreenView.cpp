#include "SplitScreenView.h"

#include "../Components/Transform.h"

diji::SplitScreenView::SplitScreenView(const sf::FloatRect& viewport)
{
   m_View.setViewport(viewport);
}

void diji::SplitScreenView::Update()
{
    if (m_IsFollowing)
        m_View.setCenter(m_TargetTransform->GetPosition() + m_Offset);
}
