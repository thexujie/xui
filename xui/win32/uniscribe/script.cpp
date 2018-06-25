#include "stdafx.h"
#include "script.h"
#include "win32/win32.h"
#include <algorithm>
#include "core/app.h"

#undef max
#undef min

namespace win32::uniscribe
{
    using namespace core;


    static bool HasMissingGlyphs(const uint16_t * pGlyphs, int32_t iLength, const SCRIPT_FONTPROPERTIES & sfp)
    {
        uint16_t wDefault = sfp.wgDefault;
        uint16_t wInvalid = sfp.wgInvalid;
        uint16_t wBlank = sfp.wgBlank;
        uint16_t wGlyph = 0;
        for (int32_t cnt = 0; cnt < iLength; ++cnt)
        {
            wGlyph = pGlyphs[cnt];
            if (wGlyph == wDefault ||
                (wGlyph == wInvalid && wInvalid != wBlank))
                return true;
        }
        return false;
    }

    ScriptItem::ScriptItem()
    {
        _service = std::dynamic_pointer_cast<ScriptService>(app().GetService("ScriptService"));
    }

    ScriptItem::~ScriptItem()
    {
    }

    void ScriptItem::SetText(std::wstring text)
    {
        _text = text;
    }

    core::error_e ScriptItem::Itemize()
    {
        if (!_service)
            return error_state;

        _clusters.clear();

        SCRIPT_DIGITSUBSTITUTE sds = { 0 };
        ScriptRecordDigitSubstitution(LOCALE_USER_DEFAULT, &sds);
        SCRIPT_CONTROL sc = { 0 };
        SCRIPT_STATE ss = { 0 };
        ScriptApplyDigitSubstitution(&sds, &sc, &ss);

        std::vector<SCRIPT_ITEM> scriptItems(_text.length() + 1);
        int32_t numScriptItems = 0;
        _hr = ScriptItemize(_text.c_str(), _text.length(), _text.length() + 1, &sc, &ss, scriptItems.data(), &numScriptItems);

        _items.reserve(numScriptItems);
        _items.resize(numScriptItems);
        int32_t cluster_num = 0;
        for (int32_t iitem = 0; iitem < numScriptItems; ++iitem)
        {
            SCRIPT_ITEM & siCurr = scriptItems[iitem];
            SCRIPT_ITEM & siNext = scriptItems[iitem + 1];

            scp_item & item = _items[iitem];
            item.sa = siCurr.a;
            item.trange = { siCurr.iCharPos, siNext.iCharPos - siCurr.iCharPos };
            item.crange = { cluster_num, 0 };
#ifdef _DEBUG
            item._text = _text.substr(siCurr.iCharPos, siNext.iCharPos - siCurr.iCharPos);
#endif
            std::vector<SCRIPT_LOGATTR> attrs(item.trange.length);
            _hr = ScriptBreak(_text.c_str() + item.trange.index, item.trange.length, &item.sa, attrs.data());

            for (int32_t itext = 0; itext < attrs.size(); ++itext)
            {
                scp_cluster cluster;
                ++cluster_num;

                cluster.index = _clusters.size();
                cluster.trange = { item.trange.index + itext, 1 };
                cluster.grange = {};

                const SCRIPT_LOGATTR & attr_first = attrs[itext];
                if (attr_first.fSoftBreak)
                    cluster.flags |= scp_cluster::softbreak;
                if (attr_first.fWhiteSpace)
                    cluster.flags |= scp_cluster::whitespace;

                // check the next char.
                while (itext < attrs.size() - 1)
                {
                    const SCRIPT_LOGATTR & tattr = attrs[itext + 1];

                    if (tattr.fCharStop || tattr.fInvalid)
                        break;

                    if (tattr.fSoftBreak)
                        cluster.flags |= scp_cluster::softbreak;
                    if (tattr.fWhiteSpace)
                        cluster.flags |= scp_cluster::whitespace;

                    ++cluster.trange.length;
                    ++itext;
                }

                _clusters.emplace_back(cluster);
                ++item.crange.length;
            }
        }

#ifdef _DEBUG
        for (int32_t icluster = 0; icluster < _clusters.size(); ++icluster)
        {
            scp_cluster & cluster = _clusters[icluster];
            cluster._text = _text.substr(cluster.trange.index, cluster.trange.length);
        }
#endif
        return error_ok;
    }

    core::error_e ScriptItem::Slice()
    {
        if (!_service)
            return error_state;

        _runs.clear();
        _views.clear();
        _lines.clear();

        for (int32_t iitem = 0; iitem < _items.size(); ++iitem)
        {
            const scp_item & item = _items[iitem];
            int32_t ich = item.crange.index;
            int32_t ich_end = item.crange.index + item.crange.length;
            while (ich < ich_end)
            {
                scp_run run;
                run.index = _runs.size();
                run.item = iitem;
                run.sa = item.sa;
                run.trange = {};
                run.crange = {};
                run.rrange = {};

                for (; ich < ich_end; ++ich)
                {
                    scp_cluster & cluster = _clusters[ich];
                    cluster.run = run.index;
                    // first cluster
                    if (!run.crange.length)
                    {
                        run.font = cluster.format.font;
                        run.trange = cluster.trange;
                        run.crange = { ich, 1 };
                    }
                    else
                    {
                        if (cluster.format.font != run.font)
                            break;

                        run.trange.length += cluster.trange.length;
                        ++run.crange.length;
                    }
                }

                _runs.emplace_back(run);
            }
        }

#ifdef _DEBUG
        for (int32_t irun = 0; irun < _runs.size(); ++irun)
        {
            scp_run & run = _runs[irun];
            run._text = _text.substr(run.trange.index, run.trange.length);
        }
#endif
        return error_ok;
    }

    core::error_e ScriptItem::Shape()
    {
        if (!_service)
            return error_state;

        _glyphs.clear();
        _glyph_attrs.clear();
        _glyph_advances.clear();
        _glyph_offsets.clear();
        _cluster_indices.clear();

        // font fallback
        enum fallback_e
        {
            // 等待字体回退
            fallback_waiting = 0,
            // 已检测到有无法显示的字符.
            fallback_prepare,
            // 正在使用后备字体进行 shape
            fallback_falling,
            // 无法回退到任何一个可用的字体，使用默认字体的 missing char.
            fallback_missing,
            // 用 missing char 代替了
            fallback_abandon,
        };

        HDC hdc = _service->GetHDC();
        view_font_t font = _service->GetFont(0);
        HGDIOBJ hOldFont = ::SelectObject(hdc, font.hfont);
        int32_t err = 0;

        SCRIPT_FONTPROPERTIES fprop = { sizeof(SCRIPT_FONTPROPERTIES) };

        int32_t goffset = 0;
        for (int32_t irun = 0; irun < _runs.size(); ++irun)
        {
            scp_run & run = _runs[irun];

            int32_t ifont_new = -1;
            font = _service->GetFont(run.font);
            ::SelectObject(hdc, font.hfont);

            int32_t gcount_max = run.trange.length * 3 / 2 + 16;
            int32_t gcount = 0;

            _glyphs.resize(goffset + gcount_max);
            _glyph_attrs.resize(goffset + gcount_max);
            _cluster_indices.resize(run.trange.index + run.trange.length);

            fallback_e fallback = fallback_waiting;
            while (true)
            {
                // 获取 glyph 信息，包括 glyph id、text cluster、glyph attr.
                err = ScriptShape(hdc,
                    font.cache,
                    _text.c_str() + run.trange.index, run.trange.length,
                    gcount_max, &(run.sa),
                    _glyphs.data() + goffset, 
                    _cluster_indices.data() + run.trange.index,
                    _glyph_attrs.data() + goffset, 
                    &gcount);

                if (err == E_PENDING)
                {
                    // 仅当没有 SelectObject(mhdc, font.hfont) 时出现，这是不可能的。
                    err = S_OK;
                }
                else if (err == E_OUTOFMEMORY)
                {
                    return error_outofmemory;
                }
                else if (err == USP_E_SCRIPT_NOT_IN_FONT)
                {
                    if (fallback == fallback_abandon)
                        err = S_OK; // 实在不行了，连 missing char 都没有...
                    else
                        fallback = fallback_prepare;
                }
                else if (err == S_OK)
                {
                    if (fallback == fallback_waiting || fallback == fallback_falling)
                    {
                        ScriptGetFontProperties(hdc, font.cache, &fprop);
                        if (HasMissingGlyphs(_glyphs.data() + goffset, gcount, fprop))
                        {
                            if (fallback == fallback_waiting)
                                fallback = fallback_prepare;
                            else
                                fallback = fallback_missing;
                            err = USP_E_SCRIPT_NOT_IN_FONT;
                        }
                        else
                        {
                        }
                    }
                }
                else
                    err = S_OK; // 无法处理的情况

                if (err >= 0)
                    break;

                // 需要查找后备字体
                if (fallback == fallback_prepare)
                {
                    graphics::text::font fontfb = _service->GetFontFallBack(font.font, run.sa.eScript, _text.c_str() + run.trange.index, run.trange.length);
                    if (!fontfb.family.empty())
                    {
                        ifont_new = _service->MapFont(fontfb);
                        font = _service->GetFont(ifont_new);
                        // 查找到了后备字体，将尝试使用后备字体进行 shape.
                        ::SelectObject(hdc, font.hfont);
                        fallback = fallback_falling;
                    }
                    else
                    {
                        // 没有找到后备字体，用默认字体的 missing char.
                        fallback = fallback_missing;
                    }
                }
                else
                {
                }

                if (fallback == fallback_missing)
                {
                    font = _service->GetFont(run.font);
                    ::SelectObject(hdc, font.hfont);
                    ifont_new = run.font;
                    run.sa.eScript = SCRIPT_UNDEFINED;
                    fallback = fallback_abandon;
                }
                else
                {
                }
            }

            if(ifont_new >= 0)
                run.font = ifont_new;

            _glyphs.resize(goffset + gcount);
            _glyph_attrs.resize(goffset + gcount);

            // 获取 advances 和 offsets.
            _glyph_advances.resize(goffset + gcount);
            _glyph_offsets.resize(goffset + gcount);

            ABC abc = {};
            _hr = ScriptPlace(hdc, font.cache,
                _glyphs.data() + goffset, gcount, _glyph_attrs.data() + goffset, 
                &run.sa, 
                _glyph_advances.data() + goffset, _glyph_offsets.data() + goffset, 
                &abc);

            run.grange = { goffset, gcount };
            goffset += gcount;
            //runitem.advance = abc.abcA + abc.abcB + abc.abcC;
        }

        SelectObject(hdc, hOldFont);


        // 计算出每个 cluster 的 glyph 范围
        for (int32_t irun = 0; irun < _runs.size(); ++irun)
        {
            scp_run & run = _runs[irun];

            int32_t iglyph_beg = run.grange.index;
            int32_t iglyph_end = run.grange.index + run.grange.length;
            int32_t iglyph_inc = 1;
            if (run.sa.fRTL)
            {
                iglyph_beg = run.grange.index + run.grange.length - 1;
                iglyph_end = run.grange.index - 1;
                iglyph_inc = -1;
            }

            for (int32_t iglyph = iglyph_beg, icluster = 0; iglyph != iglyph_end;)
            {
                scp_cluster & cluster = _clusters[run.crange.index + icluster];
                if (cluster.grange.length != 0)
                {
                    SCRIPT_VISATTR & glyph_attr = _glyph_attrs[iglyph];
                    if (glyph_attr.fClusterStart)
                    {
                        ++icluster;
                        continue;
                    }
                }
                else
                {
                    // 可能是 fallback 下来的，跟 scp 中不一定相同，从 run 中获取
                    cluster.format.font = run.font;
                    cluster.grange.index = iglyph;
                }
                cluster.grange.length += iglyph_inc;
                iglyph += iglyph_inc;
            }
        }

        // 计算每个 cluster 的大小
        for (int32_t icluster = 0; icluster < _clusters.size(); ++icluster)
        {
            scp_cluster & cluster = _clusters[icluster];
            if (cluster.grange.length < 0)
            {
                cluster.grange.index = cluster.grange.index + cluster.grange.length + 1;
                cluster.grange.length = -cluster.grange.length;
            }

            cluster.height = _service->GetFont(cluster.format.font).metrics.height;
            for (int32_t iglyph = 0; iglyph < cluster.grange.length; ++iglyph)
                cluster.advance += _glyph_advances[cluster.grange.index + iglyph];
        }

        return error_ok;
    }


    void ScriptItem::Layout(int32_t start, int32_t width, wrapmode_e wrapmode)
    {
        _views.clear();
        _lines.clear();
        if (!_clusters.size())
            return;

        int32_t iadvance = start;

        int32_t icluster_last = 0;
        int32_t icluster_break = 0;

        int32_t iline_curr = 0;

        for (int32_t irun = 0, icluster = 0; irun < _runs.size(); ++irun)
        {
            scp_run & runitem = _runs[irun];
            icluster_break = runitem.crange.index;

            for (/*icluster = runitem.crange.index*/; icluster < runitem.crange.index + runitem.crange.length; ++icluster)
            {
                scp_cluster & cluster = _clusters[icluster];
                cluster.run = irun;
                if (cluster.flags & (scp_cluster::softbreak | scp_cluster::whitespace))
                    icluster_break = icluster;

                if (width < 0 || icluster - icluster_last < 1 || iadvance + cluster.advance < width)
                {
                    // 不用换行
                }
                else
                {
                    if (wrapmode == wrapmode_char || icluster - icluster_last == 1 || icluster_break <= icluster_last)
                    {
                        scp_line line;
                        line.line = iline_curr++;
                        line.crange = { icluster_last, icluster - icluster_last };
                        _lines.push_back(line);
                        icluster_last = icluster;
                        iadvance = 0;
                    }
                    else if (wrapmode == wrapmode_word)
                    {
                        scp_line line;
                        line.line = iline_curr++;
                        line.crange = { icluster_last, icluster_break - icluster_last };
                        _lines.push_back(line);
                        icluster_last = icluster_break;
                        iadvance = 0;

                        for (int cnt = icluster_break; cnt < icluster; ++cnt)
                        {
                            scp_cluster & temp = _clusters[cnt];
                            iadvance += temp.advance;
                        }
                    }
                    else {}
                }

                iadvance += cluster.advance;
            }

            if(irun == _runs.size() - 1 && icluster > icluster_last)
            {
                scp_line line;
                line.line = iline_curr;
                line.crange = { icluster_last, icluster - icluster_last};
                _lines.push_back(line);
            }
        }

        //-- 生成 views
        for (int32_t iline = 0; iline < _lines.size(); ++iline)
        {
            scp_line & line = _lines[iline];
            if (!line.crange.length)
                continue;

            line.vrange.index = _views.size();
            scp_view view;
            view.index = _views.size();
            view.line = iline;
            view.run = -1;
            view.color = -1;
            for (int32_t icluster = 0; icluster < line.crange.length; ++icluster)
            {
                const scp_cluster & cluster = _clusters[line.crange.index + icluster];
                if((cluster.run != view.run || cluster.format.color != view.color) && view.crange.length)
                {
                    _views.push_back(view);
                    view.index = _views.size();
                    view.line = iline;
                    view.run = -1;
                    view.color = -1;
                    view.crange = {};
                    view.grange = {};
                    view.width = 0;
                }

                view.run = cluster.run;
                view.color = cluster.format.color;
                view.crange += {cluster.index, 1};
                view.grange += cluster.grange;
                view.width += cluster.advance;
            }

            if (view.crange.length)
            {
                _views.push_back(view);
                view.index = _views.size();
                view.line = iline;
                view.run = -1;
                view.color = -1;
                view.crange = {};
                view.grange = {};
                view.width = 0;
            }

            line.vrange.length = _views.size() - line.vrange.index;
        }

#ifdef _DEBUG
        for (int32_t iview = 0; iview < _views.size(); ++iview)
        {
            scp_view & view = _views[iview];
            const scp_cluster & cluster_beg = _clusters[view.crange.index];
            const scp_cluster & cluster_end = _clusters[view.crange.index + view.crange.length - 1];
            view._text = _text.substr(cluster_beg.trange.index, cluster_end.trange.index - cluster_beg.trange.index + cluster_end.trange.length);
        }
#endif

        //--
        for (int32_t iline = 0; iline < _lines.size(); ++iline)
        {
            scp_line & line = _lines[iline];
            if(line.crange.length > 0)
            {
                const scp_cluster & cluster_beg = _clusters[line.crange.index];
                const scp_cluster & cluster_end = _clusters[line.crange.index + line.crange.length - 1];
                for (int32_t icluster = line.crange.index; icluster < line.crange.index + line.crange.length; ++icluster)
                {
                    line.width += _clusters[icluster].advance;
                    line.height = std::max(line.height, _clusters[icluster].height);
                }
#ifdef _DEBUG
                line._text = _text.substr(cluster_beg.trange.index, cluster_end.trange.index + cluster_end.trange.length - cluster_beg.trange.index);
#endif
            }
        }

        for (int32_t iline = 0; iline < _lines.size(); ++iline)
        {
            scp_line & line = _lines[iline];
            std::vector<uint8_t> eles(line.vrange.length);
            std::vector<int32_t> visualOrders(line.vrange.length);

            for (int32_t iview = 0; iview < line.vrange.length; ++iview)
                eles[iview] = _runs[_views[line.vrange.index + iview].run].sa.s.uBidiLevel;

            ScriptLayout(line.vrange.length, eles.data(), visualOrders.data(), NULL);

            std::sort(_views.begin() + line.vrange.index, _views.begin() + line.vrange.index + line.vrange.length, 
                [&visualOrders, &line](const scp_view & lhs, const scp_view & rhs)
            {
                return visualOrders[lhs.index - line.vrange.index] < visualOrders[rhs.index - line.vrange.index];
            });
        }
    }

    void ScriptItem::Draw(HDC hdc, int32_t x, int32_t y, core::rc32i rect) const
    {
        ::SetBkMode(hdc, TRANSPARENT);
        HGDIOBJ hOldFont = nullptr;
        int32_t drawX = x;
        int32_t drawY = y;
        RECT rc = { rect.x, rect.y, rect.right(), rect.bottom() };

        for(int32_t iline = 0; iline < _lines.size(); ++iline)
        {
            const scp_line & line = _lines[iline];
            if (!line.crange.length)
                continue;

            int32_t run_x = 0;
            for(int32_t iview = 0; iview < line.vrange.length; ++iview)
            {
                const scp_view & view = _views[line.vrange.index + iview];

                const scp_run & run = _runs[view.run];
                const scp_item & item = _items[run.item];
                view_font_t font = _service->GetFont(run.font);
                if (!hOldFont)
                    hOldFont = ::SelectObject(hdc, font.hfont);
                else
                    ::SelectObject(hdc, font.hfont);

                ::SetTextColor(hdc, view.color);

                HRESULT hResult = ScriptTextOut(hdc, font.cache, drawX + run_x, drawY, rect.empty() ? 0 : ETO_CLIPPED, NULL,
                    &item.sa, nullptr, 0,
                    _glyphs.data() + view.grange.index, view.grange.length,
                    _glyph_advances.data() + view.grange.index,
                    nullptr,
                    _glyph_offsets.data() + view.grange.index);

                run_x += view.width;
            }

            drawX = x;
            drawY += line.height;
            if (!rect.empty() && drawY > rect.bottom())
                break;
        }

        if (hOldFont)
            ::SelectObject(hdc, hOldFont);
    }

    void ScriptItem::SetFont(int32_t index, int32_t length, const font & font)
    {
        if (!_service)
            return;

        int32_t ifont = _service->MapFont(font);
        for (int32_t icluster = index; icluster < index + length; ++icluster)
            _clusters[icluster].format.font = ifont;
    }

    void ScriptItem::SetColor(int32_t index, int32_t length, int32_t color)
    {
        for (int32_t icluster = index; icluster < index + length; ++icluster)
            _clusters[icluster].format.color = color;
    }

    void ScriptItem::Clear()
    {
        
    }

    core::si32i ScriptItem::Size() const
    {
        if (_lines.empty())
            return {};

        core::si32i size;
        for (auto & line : _lines)
        {
            size.cx = std::max(size.cx, line.width);
            size.cy += line.height;
        }
        return size;
    }
}
