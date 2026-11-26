#pragma once
#include "GridMenuItem.h"

namespace superMarioBros
{
    class BlockSelector final : public GridMenuItem
    {
    public:
        using GridMenuItem::GridMenuItem;

        [[nodiscard]] bool Select() override;

        void SetSelector(Selector* selector) { m_Selector = selector; }
        void SelectNextOrPreviousBlock(bool isNext);
        void CopyBlockHoveredPosition() const;

    private:
        Selector* m_Selector = nullptr;
    };
}
