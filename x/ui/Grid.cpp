#include "stdafx.h"
#include "Grid.h"
#include "controls/ScrollBar.h"

namespace ui
{
    Grid::Grid()
    {
    }

    Grid::~Grid()
    {

    }

    void Grid::setRows(const std::initializer_list<core::dimensionf> & rows)
    {
        _row_sizes.assign(rows);
    }

    void Grid::setCols(const std::initializer_list<core::dimensionf> & cols)
    {
        _col_sizes.assign(cols);
    }

    void Grid::addControl(size_t row, size_t col, std::shared_ptr<Control> control)
    {
        _items.push_back({row, col, control });
        Container::addControl(control);
    }

    core::sizef Grid::contentSize() const
    {
        std::vector<float32_t> widths(_col_sizes.size());
        std::vector<float32_t> heights(_row_sizes.size());

        for (auto & item : _items)
        {
            auto psize = item.control->prefferSize({});
            widths[item.col] = std::max(widths[item.col], psize.cx);
            heights[item.row] = std::max(heights[item.row], psize.cy);
        }

        core::sizef size;
        for (auto cx : widths)
            size.cx += cx;
        for (auto cy : heights)
            size.cy += cy;
        return size;
    }

    void Grid::layout(layout_flags flags)
    {
        if (!_invalid_layout && !flags.any(layout_flag::force))
            return;

        if(flags.any(layout_flag::resize_cx | layout_flag::resize_cy))
            _splitGrid(true);

        auto pbox = paddingBox();
        core::rectf box = paddingBox();
        std::vector<float32_t> widths;
        std::vector<float32_t> heights;

        for (auto & cell : _cells)
        {
            for (auto & control : cell.controls)
            {
                auto lo = control->layoutOrigin();
                if (lo != layout_origin::layout && lo != layout_origin::sticky)
                    continue;

                auto expect_size = control->prefferSize({});
                auto m = control->realMargin();
                control->place(cell.rect.offseted(pbox.pos), cell.rect.size);
            }
        }
    }

    void Grid::_splitGrid(bool force) const
    {
        std::vector<float32_t> widths;
        std::vector<float32_t> heights;

        auto pbox = paddingBox();
        if (!force && !_cells.empty())
            return;

        _cells.resize(_row_sizes.size() * _col_sizes.size(), {});

        for (auto & item : _items)
        {
            auto & cell = _cells[item.row * _col_sizes.size() + item.col];
            cell.controls.push_back(item.control);
        }

        for (size_t row = 0; row < _row_sizes.size(); ++row)
        {
            float32_t height = 0;
            auto & row_size = _row_sizes[row];
            if (row_size.avi())
            {
                if (row_size.unit == core::unit::per)
                    height = pbox.height * row_size.value / 100.0f;
                else
                    height = calc(row_size);
            }
            else
            {
                for (size_t col = 0; col < _col_sizes.size(); ++col)
                {
                    auto & cell = _cells[row * _col_sizes.size() + col];
                    for(auto & control : cell.controls)
                        height = std::max(height, control->prefferSize({}).cy);
                }
            }

            for (size_t col = 0; col < _col_sizes.size(); ++col)
            {
                auto & cell = _cells[row * _col_sizes.size() + col];
                cell.rect.height = height;
                cell.row = row;
                cell.col = col;
            }
        }

        for (size_t col = 0; col < _col_sizes.size(); ++col)
        {
            float32_t width = 0;
            auto & col_size = _col_sizes[col];
            if (col_size.avi())
            {
                if (col_size.unit == core::unit::per)
                    width = pbox.width * col_size.value / 100.0f;
                else
                    width = calc(col_size);
            }
            else
            {
                for (size_t row = 0; row < _row_sizes.size(); ++row)
                {
                    auto & cell = _cells[row * _col_sizes.size() + col];
                    for (auto & control : cell.controls)
                        width = std::max(width, control->prefferSize({}).cx);
                }
            }

            for (size_t row = 0; row < _row_sizes.size(); ++row)
            {
                auto & cell = _cells[row * _col_sizes.size() + col];
                cell.rect.width = width;
            }
        }

        float32_t cell_y = 0;
        for (size_t row = 0; row < _row_sizes.size(); ++row)
        {
            float32_t cell_x = 0;
            auto & row_header = _cells[row * _col_sizes.size() + 0];
            for (size_t col = 0; col < _col_sizes.size(); ++col)
            {
                auto & cell = _cells[row * _col_sizes.size() + col];
                cell.rect.x = cell_x;
                cell.rect.y = cell_y;

                cell_x += cell.rect.cx;
            }
            cell_y += row_header.rect.cy;
        }
    }
}

