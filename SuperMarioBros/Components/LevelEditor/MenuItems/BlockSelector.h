#pragma once
#include "GridMenuItem.h"

namespace superMarioBros
{
    class BlockSelector final : public GridMenuItem
    {
    public:
        using GridMenuItem::GridMenuItem;

        [[nodiscard]] bool Select() override;
        [[nodiscard]] bool IsExitingMenuOnClose() const override { return true; }

        void SetSelector(Selector* selector) { m_Selector = selector; }
        void CopyBlockHoveredPosition() const;

    private:
        Selector* m_Selector = nullptr;
    };
}
