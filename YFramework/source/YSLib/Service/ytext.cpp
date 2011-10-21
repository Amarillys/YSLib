﻿/*
	Copyright (C) by Franksoft 2009 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ytext.cpp
\ingroup Service
\brief 基础文本显示。
\version r6733;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2009-11-13 00:06:05 +0800;
\par 修改时间:
	2011-10-13 16:34 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::UI::YText;
*/


#include "ytext.h"
#include "yblit.h"

using namespace ystdex;

YSL_BEGIN

using namespace Drawing;
using namespace Text;

YSL_BEGIN_NAMESPACE(Drawing)

TextState::TextState()
	: PenStyle(FetchDefaultFontFamily()), Margin(),
	PenX(0), PenY(0), LineGap(0)
{}
TextState::TextState(Drawing::Font& font)
	: PenStyle(font.GetFontFamily()), Margin(),
	PenX(0), PenY(0), LineGap(0)
{}
TextState::TextState(FontCache& fc)
	: PenStyle(*fc.GetDefaultTypefacePtr()->GetFontFamilyPtr()), Margin(),
	PenX(0), PenY(0), LineGap(0)
{}

/*Typeface*
TextState::SetFont(const char* name, CPATH filename)
{
	return pFont = GetCache().SetFont(name, filename);
}*/

void
TextState::PutNewline()
{
	PenX = Margin.Left;
	PenY += GetTextLineHeightExOf(*this);
}

void
TextState::ResetPen()
{
	PenX = Margin.Left;
	//	PenY = Margin.Top + GetTextLineHeightExOf(*this);
	//	PenY = Margin.Top + pCache->GetAscender();
	SetCurrentTextLineNOf(*this, 0);
}

void
TextState::ResetForBounds(const Rect& r, const Size& s, const Padding& m)
{
	Margin = FetchMargin(r + m, s);
	yunsequenced(PenX = r.X + m.Left,
		PenY = r.Y + GetCache().GetAscender() + m.Top);
}


void
SetCurrentTextLineNOf(TextState& s, u16 n)
{
	s.PenY = s.Margin.Top + s.GetCache().GetAscender()
		+ GetTextLineHeightExOf(s) * n;
}

void
MovePen(TextState& ts, ucs4_t c)
{
	FontCache& cache(ts.GetCache());
	CharBitmap sbit(cache.GetGlyph(c));

	ts.PenX += cache.GetAdvance(c, sbit);
}


void
RenderChar(const Graphics& g, TextState& ts, ucs4_t c)
{
	if(!g.GetBufferPtr())
		//无缓冲区时无法绘图。
		return;

	FontCache& cache(ts.GetCache());
	CharBitmap sbit(cache.GetGlyph(c));
	const int tx(ts.PenX + cache.GetAdvance(c, sbit));

	if(std::iswgraph(c) && sbit.GetBuffer())
	{
		const int dx(ts.PenX + sbit.GetLeft()),
			dy(ts.PenY - sbit.GetTop()),
			xmin(vmax<int>(0, ts.Margin.Left - dx)),
			ymin(vmax<int>(0, ts.Margin.Top - dy));

		Blit<BlitBlendLoop, false, false>(g.GetBufferPtr(), g.GetSize(),
			MonoIteratorPair(pseudo_iterator<const PixelType>(
			ts.Color | BITALPHA), sbit.GetBuffer()),
			Size(sbit.GetWidth(), sbit.GetHeight()),
			Point(vmax<int>(ts.Margin.Left, dx), vmax<int>(ts.Margin.Top, dy)),
			Point(xmin, ymin),
			Size(vmax<int>(vmin<int>(g.GetWidth() - ts.Margin.Right - dx,
				sbit.GetWidth()) - xmin, 0), vmax<int>(vmin<int>(g.GetHeight()
				- ts.Margin.Bottom - dy, sbit.GetHeight()) - ymin, 0)));
	}
	//移动笔。
	ts.PenX = tx;
}

namespace
{
	const u8 BLT_TEXT_ALPHA_THRESHOLD(16);
	PixelType char_color;

	template<bool _bPositiveScan>
	struct BlitTextLoop
	{
		void
		operator()(int delta_x, int delta_y,
			pair_iterator<BitmapPtr, u8*> dst_iter, u8* src_iter,
			int dst_inc, int src_inc)
		{
			for(; delta_y > 0; --delta_y)
			{
				for(int x(0); x < delta_x; ++x)
				{
					if(*src_iter >= BLT_TEXT_ALPHA_THRESHOLD)
					{
						*dst_iter.base().second = *src_iter;
						*dst_iter = char_color;
					}
					++src_iter;
					xcrease<_bPositiveScan>(dst_iter);
				}
				src_iter += src_inc;
				delta_assignment<_bPositiveScan>(dst_iter, dst_inc);
			}
		}
	};
}

void
RenderChar(BitmapBufferEx& buf, TextState& ts, ucs4_t c)
{
	if(!buf.GetBufferPtr())
		//无缓冲区时无法绘图。
		return;

	FontCache& cache(ts.GetCache());
	CharBitmap sbit(cache.GetGlyph(c));
	const int tx(ts.PenX + cache.GetAdvance(c, sbit));

	if(std::iswgraph(c) && sbit.GetBuffer())
	{
		char_color = ts.Color | BITALPHA;

		const int dx(ts.PenX + sbit.GetLeft()),
			dy(ts.PenY - sbit.GetTop()),
			xmin(vmax<int>(0, ts.Margin.Left - dx)),
			ymin(vmax<int>(0, ts.Margin.Top - dy));

		Blit<BlitTextLoop, false, false>(pair_iterator<BitmapPtr, u8*>(
			buf.GetBufferPtr(), buf.GetBufferAlphaPtr()),
			buf.GetSize(), sbit.GetBuffer(),
			Size(sbit.GetWidth(), sbit.GetHeight()),
			Point(vmax<int>(ts.Margin.Left, dx), vmax<int>(ts.Margin.Top, dy)),
			Point(xmin, ymin),
			Size(vmax<int>(vmin<int>(buf.GetWidth() - ts.Margin.Right - dx,
				sbit.GetWidth()) - xmin, 0), vmax<int>(vmin<int>(buf.GetHeight()
				- ts.Margin.Bottom - dy, sbit.GetHeight()) - ymin, 0)));
	}
	//移动笔。
	ts.PenX = tx;
}


SDst
FetchResizedBottomMargin(const TextState& ts, SDst h)
{
	const u8 t(GetTextLineHeightExOf(ts));

	return t ? ts.Margin.Bottom + (h + ts.LineGap - GetVerticalOf(ts.Margin))
		% t : 0;
}

u16
FetchResizedLineN(const TextState& ts, SDst h)
{
	YAssert(GetTextLineHeightExOf(ts) != 0,
		"Zero line height found @ TextFetchResizedLineN");

	return (h - GetVerticalOf(ts.Margin)) / GetTextLineHeightExOf(ts);
}

SPos
FetchLastLineBasePosition(const TextState& ts, SDst h)
{
	return h - ts.Margin.Bottom + ts.GetCache().GetDescender() + 1;
}


SDst
FetchCharWidth(const Font& fnt, ucs4_t c)
{
	FontCache& cache(fnt.GetCache());
	CharBitmap sbit(cache.GetGlyph(c));

	return cache.GetAdvance(c, sbit);
}


u16
ATextRenderer::GetTextLineN() const
{
	return FetchResizedLineN(GetTextState(), GetContext().GetHeight());
}
u16
ATextRenderer::GetTextLineNEx() const
{
	const auto& ts(GetTextState());

	return FetchResizedLineN(ts, GetContext().GetHeight() + ts.LineGap);
}

void
ATextRenderer::SetTextLineLast()
{
	const u16 n(GetTextLineN());

	if(n)
		SetCurrentTextLineNOf(GetTextState(), n - 1);
}

void
ATextRenderer::ClearLine(u16 l, SDst n)
{
	const auto& g(GetContext());
	const auto h(g.GetHeight());

	if(g.IsValid() && l < h)
	{
		if(n == 0 || l + n > h)
			n = h - l;
		ClearPixel(g[l], g.GetWidth() * n);
	}
}

void
ATextRenderer::ClearTextLine(u16 l)
{
	SDst h(GetTextLineHeightExOf(GetTextState()));

	ClearLine(GetTextState().Margin.Top + h * l, h);
}

void
ATextRenderer::ClearTextLineLast()
{
	TextState ts(GetTextState());
	SDst h(GetTextLineHeightExOf(ts));

	ClearLine(GetContext().GetHeight() - ts.Margin.Bottom - h, h);
}


TextRegion::TextRegion()
	: ATextRenderer(), TextState(), BitmapBufferEx()
{
	InitializeFont();
}
TextRegion::TextRegion(Drawing::Font& fnt)
	: ATextRenderer(), TextState(fnt), BitmapBufferEx()
{
	InitializeFont();
}
TextRegion::TextRegion(FontCache& fc)
	: ATextRenderer(), TextState(fc), BitmapBufferEx()
{
	InitializeFont();
}

void
TextRegion::InitializeFont()
{
	Font.SetSize(Font::DefaultSize);
	Font.UpdateSize();
	ResetPen();
}

void
TextRegion::ClearLine(u16 l, SDst n)
{
	const auto& g(GetContext());

	if(l > g.GetHeight())
		return;
	if(!n)
		--n;
	if(g.IsValid() && pBufferAlpha)
	{
		const u32 t((l + n > g.GetHeight() ? g.GetHeight() - l : n)
			* g.GetWidth());

		yunsequenced(ClearPixel(g[l], t),
			ClearPixel(&pBufferAlpha[l * g.GetWidth()], t));
	}
}

void
TextRegion::Scroll(ptrdiff_t n)
{
	if(GetHeight() > Margin.Bottom)
		Scroll(n, GetHeight() - Margin.Bottom);
}
void
TextRegion::Scroll(ptrdiff_t n, SDst h)
{
	if(pBuffer && pBufferAlpha)
	{
		const s32 t(((h + Margin.Bottom > GetHeight()
			? GetHeight() - Margin.Bottom : h)
			- Margin.Top - abs(n)) * GetWidth());

		if(n && t > 0)
		{
			u32 d(Margin.Top), s(d);

			if(n > 0)
				d += n;
			else
				s -= n;
			yunsequenced(s *= GetWidth(), d *= GetWidth());
			yunsequenced(ystdex::pod_move_n(&pBuffer[s], t, &pBuffer[d]),
				ystdex::pod_move_n(&pBufferAlpha[s], t, &pBufferAlpha[d]));
		}
	}
}


void
DrawText(const Graphics& g, TextState& ts, const String& str)
{
	TextRenderer tr(ts, g);

	PrintLine(tr, str);
}
void
DrawText(const Graphics& g, const Rect& r, const String& str, const Padding& m,
	Color color)
{
	TextState ts;

	(ts.Font = Font()).Update(); //设置默认字体。
	ts.ResetForBounds(r, g.GetSize(), m);
	ts.Color = color;
	DrawText(g, ts, str);
}
void
DrawText(TextRegion& tr, const Graphics& g, const Point& pt, const Size& s,
	const String& str)
{
	PutLine(tr, str);
	BlitTo(g.GetBufferPtr(), tr, g.GetSize(), Point::Zero, pt, s);
}

YSL_END_NAMESPACE(Drawing)

YSL_BEGIN_NAMESPACE(Text)

YSL_END_NAMESPACE(Text)

YSL_END
