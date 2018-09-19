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
        };

        struct Item
        {
            size_t row = 0;
            size_t col = 0;
            std::vector<std::shared_ptr<Control>> controls;
        };

    public:
        Grid();
        virtual ~Grid();

        void setRows(std::initializer_list<core::dimensionf> & rows);
        void setCols(std::initializer_list<core::dimensionf> & cols);

        core::si32f contentSize() const override;
        void layout(layout_flags flags) override;

    protected:
        std::vector<core::dimensionf> _rows;
        std::vector<core::dimensionf> _cols;
        std::vector<Cell> _cells;
        std::vector<Item> _items;
    };
}
