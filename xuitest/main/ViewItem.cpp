#include "stdafx.h"
#include "ViewItem.h"
#include "View.h"

namespace ui
{
    const core::vec4<core::dimenf> & ViewItem::margin() const
    {
        auto v = view();
        if (!v)
            return _margin;

        if (_margin)
            return _margin;
        else
            return v->itemMargin();
    }

    const core::vec4<core::dimenf> & ViewItem::padding() const
    {
        auto v = view();
        if (!v)
            return _padding;

        if (_padding)
            return _padding;
        else
            return v->itemPadding();
    }

    core::rectf ViewItem::marginBox() const
    {
        auto v = view();
        if (!v)
            return _box;

        auto m = v->calc(margin());
        return _box.expanded(m);
    }

    core::rectf ViewItem::contentBox() const
    {
        auto v = view();
        if (!v)
            return {};

        auto p = v->calc(padding());
        return core::rectf(_box.pos + p.bleftTop(), _size - p.bsize());
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

