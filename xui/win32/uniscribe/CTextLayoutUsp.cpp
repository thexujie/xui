#include "stdafx.h"
#include "CTextLayoutUsp.h"
#include "CUspFactory.h"

namespace win32
{
    namespace script
    {
        CTextLayoutUsp::CTextLayoutUsp(CUspFactory * pFactory)
            : m_pFactory(pFactory)
        {

        }

        CTextLayoutUsp::~CTextLayoutUsp()
        {

        }

        err_t CTextLayoutUsp::Initialize(IDocSource * pSrouce)
        {
            m_pSource = pSrouce;
            return err_ok;
        }

        IDocSource * CTextLayoutUsp::GetSource() const
        {
            return m_pSource;
        }

        err_t CTextLayoutUsp::Layout(trange_t range, int_x iWidth, paragraph_tag_e tag)
        {
            //log2(L"index = %i, length = %i", range.index, range.length);
            if (!m_pFactory || !m_pSource)
                return err_not_initialized;

            m_runs.clear();
            m_glyphs.clear();
            m_advances.clear();
            m_offsets.clear();

            m_scpclusters.clear();
            m_clusters.clear();
            m_lines.clear();

            m_tag = tag;

            doc_source_t source = m_pSource->GetDocSource();
            textformat_t format = m_pSource->GetDefFormat();

            if (range.index < 0 || range.index + range.length > source.length)
                return err_invalidarg;

            // 长度为 0,不需要 scp 了.
            if (!range.length)
            {
                Branch(iWidth);
                return err_ok;
            }

            const char_16 * text = source.text + range.index;
            int_x length = range.length;

            winerr_t err;
            err = m_pFactory->IsComplex(text, length, SIC_COMPLEX);

            vector<SCRIPT_ITEM> items(range.length + 1, range.length + 1);
            int_x nrun = 0;
            err = m_pFactory->Itemize(text, length, items, length + 1, &nrun, nullptr, nullptr);
            items.resize(nrun + 1);

            int_x goffset = 0;

            // clusters.size() <= text.length()
            // glyph_attrs.size() == glyphs.size()

            vector<uint_16> text_clusters;
            vector<SCRIPT_LOGATTR> text_attrs;

            vector<SCRIPT_VISATTR> glyph_attrs;
            SCRIPT_FONTPROPERTIES fprop = { sizeof(SCRIPT_FONTPROPERTIES) };

            m_runs.resize(nrun);
            for (int_32 irun = 0; irun < nrun; ++irun)
            {
                usp_run_t & run = m_runs[irun];
                SCRIPT_ITEM & item = items[irun];
                SCRIPT_ITEM & item_next = items[irun + 1];

                run.trange = { item.iCharPos, item_next.iCharPos - item.iCharPos };
                run.analysis = item.a;

                int_x nglyph_max = run.trange.length * 3 / 2 + 16;
                int_x nglyph = 0;

                text_clusters.resize(run.trange.length);
                m_glyphs.resize(goffset + nglyph_max);
                glyph_attrs.resize(nglyph_max);


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

                usp_font_t font = m_pFactory->GetFont(format.font);
                m_pFactory->SetFont(font);
                fallback_e fallback = fallback_waiting;
                while (true)
                {
                    // 获取 glyph 信息，包括 glyph id、text cluster、glyph attr.
                    err = m_pFactory->Shape(
                        font.cache, text + run.trange.index, run.trange.length, &item.a, text_clusters,
                        m_glyphs + goffset, glyph_attrs, nglyph_max, &nglyph);

                    if (err == E_PENDING)
                        err = S_OK; // cache 出错，不管了.
                    else if (err == E_OUTOFMEMORY)
                        ;
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
                            m_pFactory->GetFontProperties(font.cache, &fprop);
                            if (UspHasMissingGlyphs(m_glyphs + goffset, nglyph, fprop))
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

                    if (err.succeeded())
                        break;

                    // 需要查找后备字体
                    if (fallback == fallback_prepare)
                    {
                        usp_font_t fontfb = m_pFactory->GetFontFallBack(format.font, item.a.eScript, text + run.trange.index, run.trange.length);
                        if (fontfb.hfont)
                        {
                            // 查找到了后备字体，将尝试使用后备字体进行 shape.
                            m_pFactory->SetFont(fontfb);
                            font = fontfb;
                            fallback = fallback_falling;
                        }
                        else
                        {
                            // 没有找到后备字体，用默认字体的 missing char.
                            fallback = fallback_missing;
                        }
                    }
                    else {}


                    if (fallback == fallback_missing)
                    {
                        font = m_pFactory->GetFont(format.font);
                        m_pFactory->SetFont(font);
                        item.a.eScript = SCRIPT_UNDEFINED;
                        fallback = fallback_abandon;
                    }
                    else {}

                }
                run.font = font;

                glyph_attrs.resize(nglyph);
                m_glyphs.resize(goffset + nglyph);

                // 获取 advances 和 offsets.
                m_advances.resize(goffset + nglyph);
                m_offsets.resize(goffset + nglyph);
                ABC abc = {};
                err = m_pFactory->Place(font.cache, &item.a,
                    m_glyphs + goffset, glyph_attrs, m_advances + goffset, m_offsets + goffset, nglyph,
                    &abc);
                // TODO：！！！
                //run.Width = abc.abcB;

                // 获取换行点。
                text_attrs.resize(length);
                err = m_pFactory->Break(text + run.trange.index, run.trange.length, &item.a, text_attrs);
                verify(err == S_OK);


                //------------------------------------------------------------------------------------------
                // 生成 usp_cluster.
                for (int_x iglyph = 0, itext = 0; iglyph < nglyph && itext < run.trange.length; /**/)
                {
                    if (glyph_attrs[iglyph].fClusterStart == 0)
                    {
                        // TODO:从右往左的文字 e.Script.RTL = true.
                        breakpoint();
                        //++iglyph;
                        continue;
                    }

                    const SCRIPT_LOGATTR & text_attr = text_attrs[itext];
                    scp_cluster_t & scp_cluster = m_scpclusters.add();
                    scp_cluster.run = irun;
                    scp_cluster.grange = { goffset + iglyph, 1 };
                    scp_cluster.trange = { run.trange.index + itext, 1 };
                    // usp 的 soft break 同 d2d 不一样，放在了后一个 attr 里.
                    scp_cluster.softbreak = (itext == run.trange.length - 1);
                    if (!scp_cluster.softbreak && itext < run.trange.length - 1)
                        scp_cluster.softbreak = text_attrs[itext + 1].fSoftBreak != 0;

                    scp_cluster.whitespace = text_attr.fWhiteSpace;

                    ++itext;
                    ++iglyph;
                    // 多个 tpoint 合并为一个 cluster.

                    while (itext < run.trange.length)
                    {
                        if (text_clusters[itext] != text_clusters[itext - 1])
                            break;

                        ++itext;
                        ++scp_cluster.trange.length;
                    }

                    // 多个 gpoint 合并为一个 cluster.
                    while (iglyph < nglyph)
                    {
                        if (glyph_attrs[iglyph].fClusterStart)
                            break;

                        ++iglyph;
                        ++scp_cluster.grange.length;
                    }

                    // 处理特殊字符
                    if (scp_cluster.trange.length == 1)
                    {
                        char_16 ch = text[scp_cluster.trange.index];
                        switch (ch)
                        {
                        case '\t':
                            m_advances[scp_cluster.grange.index] = (int_32)m_pFactory->GetTabSize(font, 0);
                            break;
                        case '\r':
                        case '\n':
                            scp_cluster.softbreak = true;
                            break;
                        default:
                            break;
                        }
                    }

                    // 计算 Cluster 宽度
                    for (int_x cnt = scp_cluster.grange.index; cnt < scp_cluster.grange.index + scp_cluster.grange.length; ++cnt)
                    {
                        scp_cluster.width += m_advances[cnt];
                    }
                }

                run.grange = { goffset, nglyph };
                run.crange = {};

                goffset += nglyph;
            }

            Branch(iWidth);
            return 0;
        }

        err_t CTextLayoutUsp::Branch(int_x iWidth)
        {
            m_clusters.clear();
            m_lines.clear();

            if (!m_pSource)
                return err_not_initialized;

            textformat_t format = m_pSource->GetDefFormat();

            // 空行，直接生成即可.
            if (m_scpclusters.is_empty())
            {
                m_clusters.resize(1);
                m_lines.resize(1);

                tl_cluster_t & cluster = m_clusters[0];
                tl_line_t & line = m_lines[0];

                cluster.cluster = 0;
                cluster.line = 0;
                cluster.para = 0;
                cluster.trange = { 0, GetParagraphTagLength(m_tag) };
                cluster.tlrect = { 0, 0, 0, format.font.size };
                cluster.whitespace = true;
                cluster.softbreak = false;
                cluster.paragraphtag = true;
                cluster.softhyphen = false;
                cluster.right2left = false;

                line.line = 0;
                line.trange = { 0, cluster.trange.length };
                line.crange = { 0, 1 };
                line.tlrect = { 0, 0, 0, format.font.size };

                return err_ok;
            }

            int_x ilineh = format.font.size;
            // 不换行
            if (format.wrap == TextWrapNone)
            {
                int_x ix = 0;
                for (int_x cnt = 0; cnt < m_scpclusters.size(); ++cnt)
                {
                    scp_cluster_t & scp_cluster = m_scpclusters[cnt];

                    usp_cluster_t & cluster = m_clusters.add();
                    cluster.run = scp_cluster.run;
                    cluster.grange = scp_cluster.grange;

                    cluster.cluster = cnt;
                    cluster.line = 0;
                    cluster.para = 0;
                    cluster.trange = scp_cluster.trange;
                    cluster.tlrect = { ix, 0, scp_cluster.width, ilineh };
                    cluster.softbreak = scp_cluster.softbreak;
                    cluster.whitespace = scp_cluster.whitespace;

                    ix += scp_cluster.width;
                }

                scp_cluster_t & scp_cluster_end = m_scpclusters.back();

                // 行尾的 cluster
                usp_cluster_t & cluster_line_tail = m_clusters.add();
                cluster_line_tail.run = scp_cluster_end.run;
                cluster_line_tail.grange.index = scp_cluster_end.grange.index + scp_cluster_end.grange.length;
                cluster_line_tail.grange.length = 0;

                cluster_line_tail.cluster = m_scpclusters.size();
                cluster_line_tail.line = 0;
                cluster_line_tail.para = 0;
                cluster_line_tail.trange.index = scp_cluster_end.trange.index + scp_cluster_end.trange.length;
                cluster_line_tail.trange.length = GetParagraphTagLength(m_tag);
                cluster_line_tail.tlrect = { ix, 0, 0, ilineh };

                cluster_line_tail.paragraphtag = true;
                cluster_line_tail.linetag = true;

                tl_line_t & line = m_lines.add();
                line.line = 0;
                line.para = 0;
                line.trange = { 0, scp_cluster_end.trange.index + scp_cluster_end.trange.length };
                line.crange = { 0, m_scpclusters.size() + 1 };
                line.tlrect = { 0, 0, ix, ilineh };
            }
            else
            {
                int_x ix = 0;
                int_x iline = 0;
                int_x iliney = 0;
                int_x icluster = 0;
                int_x ncluster = 0;
                int_x iscpcluster_beg = 0;
                int_x iscpcluster_end = 0;
                int_x iscpcluster_softbreak = 0;
                int_x iscpcluster_softbreak_x = 0;
                int_x iscpcluster_max = m_scpclusters.size();
                int_x nscpcluster = 0;

                for (int_x iscpcluster = 0; iscpcluster < iscpcluster_max; ++iscpcluster, ++nscpcluster)
                {
                    scp_cluster_t & scp_cluster = m_scpclusters[iscpcluster];
                    ix += scp_cluster.width;
                    if (!nscpcluster && iscpcluster < iscpcluster_max - 1)
                        continue;

                    if (scp_cluster.softbreak)
                    {
                        iscpcluster_softbreak = iscpcluster;
                        iscpcluster_softbreak_x = ix;
                    }
                    else {}

                    // 要保证最后的一个字符被纳入最后一行.
                    bool new_line = true;
                    if (iscpcluster < iscpcluster_max - 1)
                    {
                        if (ix + m_scpclusters[iscpcluster + 1].width < iWidth)
                            new_line = false;
                    }
                    else
                    {
                        // 最后一个字符了，必须生成新行了，由于并未超出 layout width，不理会 soft break.
                        new_line = true;
                        iscpcluster_softbreak = 0;
                        iscpcluster_softbreak_x = 0;
                    }
                    if (!new_line)
                        continue;

                    // ---------------------已经超过了布局宽度，根据换行方式换行.
                    if (format.wrap == TextWrapChar)
                    {
                        iscpcluster_end = iscpcluster;
                        ix = 0;
                    }
                    else if (format.wrap == TextWrapWord)
                    {
                        if (iscpcluster_softbreak > 0)
                        {
                            iscpcluster_end = iscpcluster_softbreak;
                            iscpcluster_softbreak = 0;
                            ix = ix - iscpcluster_softbreak_x;
                        }
                        else
                        {
                            iscpcluster_end = iscpcluster;
                            ix = 0;
                        }
                    }
                    else if (format.wrap == TextWrapWordNoBreaking)
                    {
                        if (iscpcluster_softbreak > 0)
                        {
                            iscpcluster_end = iscpcluster_softbreak;
                            iscpcluster_softbreak = 0;
                            ix = ix - iscpcluster_softbreak_x;
                        }
                        // no breaking，等待下一个 softbreak.
                        else if (iscpcluster < iscpcluster_max - 1)
                            continue;
                        // 必须生成新行了
                        else
                        {
                            iscpcluster_end = iscpcluster;
                            ix = 0;
                        }
                    }
                    else
                    {
                        iscpcluster_end = iscpcluster;
                        ix = 0;
                    }

                    int_x ix_temp = 0;
                    for (int_x cnt = iscpcluster_beg; cnt <= iscpcluster_end; ++cnt)
                    {
                        scp_cluster_t & scp_cluster = m_scpclusters[cnt];
                        usp_cluster_t & cluster = m_clusters.add();
                        cluster.run = scp_cluster.run;
                        cluster.grange = scp_cluster.grange;

                        cluster.cluster = ncluster++;
                        cluster.line = iline;
                        cluster.para = 0;
                        cluster.trange = scp_cluster.trange;
                        cluster.tlrect = { ix_temp, iliney, scp_cluster.width, ilineh };
                        cluster.softbreak = scp_cluster.softbreak;

                        ix_temp += scp_cluster.width;
                    }

                    scp_cluster_t & scp_cluster_beg = m_scpclusters[iscpcluster_beg];
                    scp_cluster_t & scp_cluster_end = m_scpclusters[iscpcluster_end];

                    // 行尾的 cluster
                    usp_cluster_t & cluster_line_tail = m_clusters.add();
                    cluster_line_tail.run = scp_cluster_end.run;
                    cluster_line_tail.grange.index = scp_cluster_end.grange.index + scp_cluster_end.grange.length;
                    cluster_line_tail.grange.length = 0;

                    cluster_line_tail.cluster = ncluster++;
                    cluster_line_tail.line = iline;
                    cluster_line_tail.para = 0;
                    cluster_line_tail.trange.index = scp_cluster_end.trange.index + scp_cluster_end.trange.length;
                    if (iscpcluster == iscpcluster_max - 1)
                    {
                        cluster_line_tail.paragraphtag = true;
                        cluster_line_tail.trange.length = GetParagraphTagLength(m_tag);
                    }
                    else
                        cluster_line_tail.trange.length = 0;
                    cluster_line_tail.tlrect = { ix_temp, iliney, 0, ilineh };

                    cluster_line_tail.linetag = true;

                    tl_line_t & line = m_lines.add();
                    line.line = iline++;
                    line.para = 0;
                    line.trange = { scp_cluster_beg.trange.index,
                        scp_cluster_end.trange.index + scp_cluster_end.trange.length - scp_cluster_beg.trange.index };
                    line.crange = { icluster, ncluster - icluster };
                    line.tlrect = { 0, iliney, ix_temp, ilineh };

                    icluster = ncluster;
                    iliney += ilineh;

                    nscpcluster = iscpcluster - iscpcluster_end;
                    iscpcluster_beg = iscpcluster_end + 1;
                }
            }

            // 更新 run
            int_x icluster = 0;
            int_x icluster_max = m_clusters.size();
            while (icluster < icluster_max)
            {
                usp_cluster_t & cluster_beg = m_clusters[icluster];
                usp_run_t & run = m_runs[cluster_beg.run];
                run.crange = { cluster_beg.cluster, 1 };
                ++icluster;
                while (icluster < icluster_max)
                {
                    usp_cluster_t & cluster = m_clusters[icluster];
                    if (cluster.run == cluster_beg.run)
                    {
                        ++run.crange.length;
                        ++icluster;
                    }
                    else
                        break;
                }
            }

#ifdef _DEBUG
            if (m_clusters.is_empty())
                log2(L"no clusters were generated!");
#endif
            return err_ok;
#if 0
            // 计算行的宽高
            iY = 0;
            for (int_x cnt = 0, len = m_lines.size(); cnt < len; ++cnt)
            {
                usp_line_t & line = m_lines[cnt];
                line.x = 0;
                line.y = iY;
                for (int_x index = 0; index < line.cluster_length; ++index)
                {
                    usp_cluster_t & cluster = m_clusters[line.cluster_index + index];
                    line.width += cluster.tlrect.w;
                    line.height = format.font.size;
                }
                iY += line.height;
            }

            // 计算 cluster 的 x, y
            iX = iY = 0;
            for (int_x iLine = 0, iLineEnd = m_lines.size(); iLine < iLineEnd; ++iLine)
            {
                usp_line_t & line = m_lines[iLine];
                for (int_x index = 0; index < line.cluster_length; ++index)
                {
                    usp_cluster_t & cluster = m_clusters[line.cluster_index + index];
                    cluster.tlrect.x = iX;
                    cluster.tlrect.y = iY;
                    cluster.line = iLine;
                    iX += cluster.tlrect.w;
                }
                iX = 0;
                iY += line.height;
            }

            m_size.w = 0;
            m_size.h = 0;
            for (int_x cnt = 0, len = m_lines.size(); cnt < len; ++cnt)
            {
                m_size.h += m_lines[cnt].height;
                m_size.w = maxof(m_lines[cnt].width, m_size.w);
            }
#endif
            return err_ok;
        }

        tl_cluster_t CTextLayoutUsp::FindCluster(int_x iIndex) const
        {
            for (int_x cnt = 0, len = m_clusters.size(); cnt < len; ++cnt)
            {
                const tl_cluster_t & cluster = m_clusters[cnt];
                if (cluster.trange.index <= iIndex && (iIndex < cluster.trange.index + cluster.trange.length || (cluster.trange.length == 0 && cluster.paragraphtag)))
                    return cluster;
            }

            throw exp_out_of_bound();
        }

        tl_cluster_t CTextLayoutUsp::GetCluster(int_x iCluster) const
        {
            return m_clusters[iCluster];
        }

        tl_line_t CTextLayoutUsp::GetLine(int_x iLine) const
        {
            return m_lines[iLine];
        }


        tl_range_t CTextLayoutUsp::HitTest(pointix point) const
        {
            int_x iLine = -1;
            if (point.y <= 0)
                iLine = 0;
            else
            {
                for (int_x iline = 0; iline < m_lines.size(); ++iline)
                {
                    const tl_line_t & line = m_lines[iline];
                    if (point.y < line.tlrect.y + line.tlrect.h)
                    {
                        iLine = iline;
                        break;
                    }
                }
                if (iLine < 0)
                    iLine = m_lines.size() - 1;
            }

            const tl_line_t & line = m_lines[iLine];
            int_x iCluster = -1;
            if (point.x < 0)
                iCluster = line.crange.index;
            else
            {
                for (int_x icluster = line.crange.index; icluster < line.crange.index + line.crange.length; ++icluster)
                {
                    const tl_cluster_t & cluster = m_clusters[icluster];
                    if (point.x < cluster.tlrect.x + cluster.tlrect.w / 2)
                    {
                        iCluster = icluster;
                        break;
                    }
                }
                if (iCluster < 0)
                {
                    iCluster = line.crange.index + line.crange.length - 1;
                }
            }

            const tl_cluster_t & cluster = m_clusters[iCluster];
            tl_range_t range;
            range.crange = { cluster.cluster, 1 };
            range.trange = cluster.trange;
            range.tlrect = cluster.tlrect;
            return range;
        }

        int_x CTextLayoutUsp::HitTestRange(int_x iCluster, int_x iLength, tl_range_t * ranges, int_x iCount) const
        {
            if (!iLength)
                return 0;

            const tl_cluster_t & cluster_beg = m_clusters[iCluster];
            const tl_cluster_t & cluster_end = m_clusters[iCluster + iLength - 1];
            // 只有一行，好办。
            if (cluster_beg.line == cluster_end.line)
            {
                if (ranges && iCount)
                {
                    tl_range_t & range = ranges[0];
                    range.para = 0;
                    range.line = 0;
                    range.trange.index = cluster_beg.trange.index;
                    range.trange.length = cluster_end.trange.index + cluster_end.trange.length - cluster_beg.trange.index;
                    range.tlrect.x = cluster_beg.tlrect.x;
                    range.tlrect.y = cluster_beg.tlrect.y;
                    range.tlrect.w = cluster_end.tlrect.x + cluster_end.tlrect.w - cluster_beg.tlrect.x;
                    range.tlrect.h = cluster_beg.tlrect.h;
                }
                return 1;
            }
            else
            {
                int_x count = cluster_end.line - cluster_beg.line + 1;
                if (!ranges || !iCount)
                    return count;

                if (iCount < count)
                    return -1;

                tl_range_t & range_beg = ranges[0];
                tl_range_t & range_end = ranges[count - 1];
                const tl_line_t & line_beg = m_lines[cluster_beg.line];
                const tl_line_t & line_end = m_lines[cluster_end.line];

                range_beg.line = line_beg.line;
                range_beg.para = 0;
                range_beg.trange = { cluster_beg.trange.index, line_beg.trange.length - cluster_beg.trange.index };
                range_beg.tlrect = { cluster_beg.tlrect.x, line_beg.tlrect.y, line_beg.tlrect.w - cluster_beg.tlrect.x, line_beg.tlrect.h };

                range_end.line = line_end.line;
                range_end.para = 0;
                range_end.trange = { line_end.trange.index, cluster_end.trange.index + cluster_end.trange.length - line_end.trange.index };
                range_end.tlrect = { line_end.tlrect.x, line_end.tlrect.y, cluster_end.tlrect.x + cluster_end.tlrect.w, line_end.tlrect.h };

                for (int_x iline = cluster_beg.line + 1; iline < cluster_end.line; ++iline)
                {
                    const tl_line_t & line = m_lines[iline];
                    int_x irange = iline - (cluster_beg.line + 1) + 1;
                    tl_range_t & range = ranges[irange];
                    range.line = line.line;
                    range.para = 0;
                    range.trange = line.trange;
                    range.crange = line.crange;
                    range.tlrect = line.tlrect;
                }
                return count;
            }
        }

        tl_metrics_t CTextLayoutUsp::GetMetrics() const
        {
            tl_metrics_t metrics = {};
            for (int_x cnt = 0; cnt < m_lines.size(); ++cnt)
            {
                const tl_line_t & line = m_lines[cnt];
                if (line.tlrect.w > metrics.tl_width)
                    metrics.tl_width = line.tlrect.w;
                metrics.tl_height += line.tlrect.h;
                metrics.length += line.trange.length;
            }
            metrics.cluster_length = m_clusters.size();
            metrics.line_length = m_lines.size();
            return metrics;
        }

        winerr_t CTextLayoutUsp::Draw(HDC hdc, int_x iX, int_x iY, uint_32 color, const rectix & rectLayout) const
        {
            uint_32 uiColor = ((color >> 16) & 0xFF) | (color & 0xFF00) | ((color << 16) & 0xFF0000);
            uint_32 uiColorOld = ::SetTextColor(hdc, uiColor);
            int iModeOld = ::SetBkMode(hdc, TRANSPARENT);
            uint_32 uiAlignOld = ::SetTextAlign(hdc, 0);
            RECT rect = { (int_32)rectLayout.x, (int_32)rectLayout.y, (int_32)rectLayout.right(), (int_32)rectLayout.bottom() };

            // 一行一行地绘制，对于跨越行的 run，只绘制本行内的 run.
            for (int_x cnt = 0, len = m_lines.size(); cnt < len; ++cnt)
            {
                const tl_line_t & line = m_lines[cnt];
                const usp_cluster_t & cluster_beg = m_clusters[line.crange.index];
                const usp_cluster_t & cluster_end = m_clusters[line.crange.index + line.crange.length - 1];
                if (cluster_end.grange.index + cluster_end.grange.length - cluster_beg.grange.index <= 0)
                {
                    iY += line.tlrect.h;
                    continue;
                }

                if (cluster_beg.run == cluster_end.run)
                {
                    const usp_run_t & run = m_runs[cluster_beg.run];
                    DrawRun(hdc, iX, iY, rect, cluster_beg.run, line.crange.index, line.crange.index + line.crange.length);
                }
                else
                {
                    const usp_run_t & run_beg = m_runs[cluster_beg.run];
                    DrawRun(hdc, iX, iY, rect, cluster_beg.run, line.crange.index, run_beg.crange.index + run_beg.crange.length);
                    for (int_x iRun = cluster_beg.run + 1; iRun < cluster_end.run; ++iRun)
                    {
                        const usp_run_t & run = m_runs[iRun];
                        DrawRun(hdc, iX, iY, rect, iRun, run.crange.index, run.crange.index + run.crange.length);
                    }
                    const usp_run_t & run_end = m_runs[cluster_end.run];
                    DrawRun(hdc, iX, iY, rect, cluster_end.run, run_end.crange.index, line.crange.index + line.crange.length);
                }
                iY += line.tlrect.h;
            }
            ::SetTextAlign(hdc, uiAlignOld);
            ::SetTextColor(hdc, uiColorOld);
            ::SetBkMode(hdc, iModeOld);
            return S_OK;
        }

        winerr_t CTextLayoutUsp::DrawRun(HDC hdc, int_x iX, int_x iY, const RECT & rect,
            int_x iRun, int_x icluster_beg, int_x icluster_end) const
        {
            if (icluster_beg >= icluster_end)
                return err_ok;

            const usp_run_t & run = m_runs[iRun];
            HGDIOBJ hFontOld = ::SelectObject(hdc, run.font.hfont);
            TEXTMETRICW tm;
            ::GetTextMetricsW(hdc, &tm);

            const usp_cluster_t & cluster_beg = m_clusters[icluster_beg];
            const usp_cluster_t & cluster_end = m_clusters[icluster_end - 1];
            int_x iglyph = cluster_beg.grange.index;
            int_x nglyph = cluster_end.grange.index + cluster_end.grange.length - cluster_beg.grange.index;
            iX += cluster_beg.tlrect.x;
            iY += tm.tmExternalLeading;
            HRESULT hResult = m_pFactory->TextOutScp(hdc, run.font.cache, iX, iY, ETO_CLIPPED, &rect,
                &run.analysis,
                m_glyphs + iglyph, nglyph,
                m_advances + iglyph,
                NULL,
                m_offsets + iglyph);
            ::SelectObject(hdc, hFontOld);
            return hResult;
        }
    }
}
