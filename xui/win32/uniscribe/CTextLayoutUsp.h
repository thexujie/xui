#pragma once

#include "UspInc.h"

VENUS_BEG

struct usp_line_t
{
	int_x line;

	int_x index;
	int_x length;

	int_x cluster_index;
	int_x cluster_length;

	int_x x;
	int_x y;
	int_x width;
	int_x height;
};

class CTextLayoutUsp : public ObjectT<ITextLayoutUsp>
{
public:
	CTextLayoutUsp(class CUspFactory * pFactory);
	~CTextLayoutUsp();

	err_t Initialize(IDocSource * pSrouce);
	IDocSource * GetSource() const;

	err_t Layout(trange_t range, int_x iWidth, paragraph_tag_e tag = paragraph_tag_none);
	err_t Branch(int_x iWidth);

	tl_cluster_t FindCluster(int_x iIndex) const;
	tl_cluster_t GetCluster(int_x iCluster) const;
	tl_line_t GetLine(int_x iLine) const;

	tl_range_t HitTest(pointix point) const;
	int_x HitTestRange(int_x iIndex, int_x iLength, tl_range_t * rects, int_x iCount) const;
	tl_metrics_t GetMetrics() const;
public:
	winerr_t Draw(HDC hdc, int_x iX, int_x iY, uint_32 color, const rectix & rect) const;
	winerr_t DrawRun(HDC hdc, int_x iX, int_x iY, const RECT & rect,
					   int_x iRun, int_x iCluster, int_x iClusterLength) const;

private:
	CUspFactory * m_pFactory;

	IDocSource * m_pSource;

	trange_t m_range;
	paragraph_tag_e m_tag;
	sizeix m_size;

	// run
	vector<usp_run_t> m_runs;
	// glyph
	vector<uint_16> m_glyphs;
	// advance
	vector<int_32> m_advances;
	// offset
	vector<GOFFSET> m_offsets;
	// scp cluster
	vector<scp_cluster_t> m_scpclusters;
	// cluster
	vector<usp_cluster_t> m_clusters;
	// line
	vector<tl_line_t> m_lines;
};

VENUS_END

