#pragma once
#include "Container.h"
#include "controls/ScrollBar.h"

namespace ui
{
    class Grid : public Container
    {
    public:
        struct Cell
        {
            size_t row = 0;
            size_t col = 0;
            size_t row_pan = 1;
            size_t col_pan = 1;
            core::rectf rect;
            std::vector<std::shared_ptr<Control>> controls;
        };

        struct Item
        {
            size_t row = 0;
            size_t col = 0;
            std::shared_ptr<Control> control;
        };

    public:
        Grid();
        virtual ~Grid();

        void setRows(const std::initializer_list<core::dimensionf> & rows);
        void setCols(const std::initializer_list<core::dimensionf> & cols);
        void addControl(size_t row, size_t col, std::shared_ptr<Control> control);

        core::sizef contentSize() const override;
        void layout(layout_flags flags) override;
        std::string styleName() const override { return "grid"; }

    private:
        void _splitGrid(bool force = false) const;

    protected:
        std::vector<core::dimensionf> _row_sizes;
        std::vector<core::dimensionf> _col_sizes;
        mutable std::vector<Cell> _cells;
        mutable std::vector<Item> _items;
    };
}
