#include "stdafx.h"
#include "Grid.h"
#include "controls/ScrollBar.h"

namespace ui
{
    Grid::Grid()
    {
        _layout_direction = core::align::none;
    }

    Grid::~Grid()
    {

    }

    void Grid::setRows(std::initializer_list<core::dimensionf> & rows)
    {
        _rows.assign(rows);
    }

    void Grid::setCols(std::initializer_list<core::dimensionf> & cols)
    {
        _cols.assign(cols);
    }

    core::si32f Grid::contentSize() const
    {
        if(_cells.empty())
        {
            auto & cells = const_cast<std::vector<Cell> &>(_cells);
            cells.resize(_rows.size() * _cols.size(), {});
            for(size_t row = 0; row < _rows.size(); ++row)
            {
                for (size_t col = 0; col < _cols.size(); ++col)
                {
                    auto & cell = cells[row * _cols.size() + col];
                    cell.row = row;
                    cell.col= row;
                }
            }
        }

        core::si32f size;
        std::vector<float32_t> widths;
        std::vector<float32_t> heights;

        for (auto & item : _items)
        {
            auto & row_size = _rows[item.row];
            auto & col_size = _rows[item.row];

            for(auto & control : item.controls)
            {
                auto lo = control->layoutOrigin();
                if (lo != layout_origin::layout && lo != layout_origin::sticky)
                    continue;

                auto expect_size = control->expectSize();
                auto m = control->realMargin();


                if (col_size.avi())
                    widths[item.col] = calc_x(col_size);
                else
                    widths[item.col] = std::max(widths[item.col], expect_size.cx + m.bwidth());

                if (row_size.avi())
                    heights[item.col] = calc_y(row_size);
                else
                    heights[item.row] = std::max(heights[item.row], expect_size.cy + m.bheight());
            }
        }

        for (auto & w : widths)
            size.cx += w;
        for (auto & h : heights)
            size.cy += h;
        return size;
    }

    void Grid::layout(layout_flags flags)
    {
        if (!_invalid_layout && !flags.any(layout_flag::force))
            return;

        core::rc32f box = paddingBox();
        std::vector<float32_t> widths;
        std::vector<float32_t> heights;

        for (auto & cell : _cells)
        {
            auto & row_size = _rows[cell.row];
            auto & col_size = _rows[cell.row];

            for (auto & control : cell.controls)
            {
                auto lo = control->layoutOrigin();
                if (lo != layout_origin::layout && lo != layout_origin::sticky)
                    continue;

                auto expect_size = control->expectSize();
                auto m = control->realMargin();


                if (col_size.avi())
                    widths[cell.col] = calc_x(col_size);
                else
                    widths[cell.col] = std::max(widths[cell.col], expect_size.cx + m.bwidth());

                if (row_size.avi())
                    heights[cell.col] = calc_y(row_size);
                else
                    heights[cell.row] = std::max(heights[cell.row], expect_size.cy + m.bheight());
            }
        }


        for (auto & cell : _cells)
        {
            for (auto & control : cell.controls)
            {
                auto lo = control->layoutOrigin();
                if (lo != layout_origin::layout && lo != layout_origin::sticky)
                    continue;

                auto expect_size = control->expectSize();
                auto m = control->realMargin();

                auto & row_size = _rows[cell.row];
                auto & col_size = _rows[cell.row];

                if (col_size.avi())
                    widths[cell.col] = calc_x(col_size);
                else
                    widths[cell.col] = std::max(widths[cell.col], expect_size.cx + m.bwidth());

                if (row_size.avi())
                    heights[cell.col] = calc_y(row_size);
                else
                    heights[cell.row] = std::max(heights[cell.row], expect_size.cy + m.bheight());
            }
        }
    }

}

