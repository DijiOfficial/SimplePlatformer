#pragma once
#include "GridMenuItem.h"

namespace superMarioBros
{
    class EnemySelector final : public GridMenuItem
    {
    public:
        using GridMenuItem::GridMenuItem;

        [[nodiscard]] bool Select() override;
        [[nodiscard]] bool IsExitingMenuOnClose() const override { return true; }

        void SetSelector(Selector* selector) { m_Selector = selector; }

    protected:
        [[nodiscard]] bool IsCellSelectable(int index) const override;

    private:
        Selector* m_Selector = nullptr;
    };
}
