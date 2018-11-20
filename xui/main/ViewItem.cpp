#include "stdafx.h"
#include "ViewItem.h"
#include "View.h"

namespace ui
{
    const core::vec4<core::dimensionf> & ViewItem::margin() const
    {
        auto v = view();
        if (!v)
            return _margin;

        if (_margin)
            return _margin;
        else
            return v->itemMargin();
    }

    const core::vec4<core::dimensionf> & ViewItem::padding() const
    {
        auto v = view();
        if (!v)
            return _padding;

        if (_padding)
            return _padding;
        else
            return v->itemPadding();
    }

    core::rectf ViewItem::contentBox() const
    {
        auto v = view();
        if (!v)
            return {};

        auto p = padding();
        return core::rectf(_box.pos + v->calc(p).bleftTop(), _size - v->calc(p).bsize());
    }

    void ViewItem::setFlag(item_flag flag, bool set)
    {
        bool s = _flags.any(flag);
        if(s != set)
        {
            _flags.set(flag, set);
            onFlagChanged(flag, set);
        }
    }
}

