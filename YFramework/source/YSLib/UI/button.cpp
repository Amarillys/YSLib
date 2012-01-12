﻿/*
	Copyright (C) by Franksoft 2010 - 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file button.cpp
\ingroup UI
\brief 样式相关的图形用户界面按钮控件。
\version r3678;
\author FrankHB<frankhb1989@gmail.com>
\since build 194 。
\par 创建时间:
	2010-10-04 21:23:32 +0800;
\par 修改时间:
	2012-01-10 21:43 +0800;
\par 文本编码:
	UTF-8;
\par 模块名称:
	YSLib::UI::Button;
*/


#include "YSLib/UI/button.h"
#include "YSLib/Service/yblit.h"
#include "YSLib/UI/ygui.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

namespace
{
	void
	RectDrawButton(const Graphics& g, Point pt, Size s, bool is_pressed = false,
		bool is_enabled = true)
	{
		YAssert(g.IsValid(), "err: @g is invalid.");

		DrawRectRoundCorner(g, pt, s, is_enabled ? Color(60, 127, 177)
			: FetchGUIShell().Colors[Styles::Workspace]);
		if(s.Width > 2 && s.Height > 2)
		{
			yunseq(pt.X += 1, pt.Y += 1, s.Width -= 2, s.Height -= 2);
			FillRect(g, pt, s, is_enabled ? Color(48, 216, 255)
				: Color(244, 244, 244));
			if(is_enabled)
			{
				if(s.Width > 2 && s.Height > 2)
				{
					Size sz(s.Width - 2, (s.Height - 2) / 2);
					Point sp(pt.X + 1, pt.Y + 1);

					FillRect(g, sp, sz, Color(232, 240, 255));
					sp.Y += sz.Height;
					if(s.Height % 2 != 0)
						++sz.Height;
					FillRect(g, sp, sz, Color(192, 224, 255));
				}
				if(is_pressed)
					TransformRect(g, pt, s,
						Drawing::transform_pixel_ex<56, 24, 32>);
			}
		}
	}
}


Thumb::Thumb(const Rect& r)
	: Control(r),
	MButton()
{
	yunseq(
		FetchEvent<Enter>(*this) += [this](TouchEventArgs&&){
			if(!bPressed)
			{
				bPressed = true;
				Invalidate(*this);
			}
		},
		FetchEvent<Leave>(*this) += [this](TouchEventArgs&&){
			if(bPressed)
			{
				bPressed = false;
				Invalidate(*this);
			}
		}
	);
}

Rect
Thumb::Refresh(const PaintContext& pc)
{
	auto enabled(IsEnabled(*this));
	const auto& g(pc.Target);
	const auto& pt(pc.Location);

	if(!enabled)
		bPressed = false;
	RectDrawButton(g, pt, GetSizeOf(*this), bPressed, enabled);
	if(enabled && IsFocused(*this))
	{
		Size s(GetSizeOf(*this));

		if(s.Width > 6 && s.Height > 6)
		{
			yunseq(s.Width -= 6, s.Height -= 6);
			DrawRect(g, pt + Vec(3, 3), s, ColorSpace::Aqua);
		}
	}
	return Rect(pt, GetSizeOf(*this));
}


Button::Button(const Rect& r, const Drawing::Font& fnt)
	: Thumb(r),
	MLabel(fnt, TextAlignment::Center)
{}

Rect
Button::Refresh(const PaintContext& pc)
{
	auto r(Thumb::Refresh(pc));

	// NOTE: partial invalidation made no efficiency improved here;
	PaintText(GetSizeOf(*this), IsEnabled(*this) ? ForeColor
		: FetchGUIShell().Colors[Styles::Workspace], PaintContext(pc.Target,
		pc.Location, Rect(pc.Location, GetSizeOf(*this))));
	return r;
}


CloseButton::CloseButton(const Rect& r)
	: Thumb(r)
{
	FetchEvent<Click>(*this) += [this](TouchEventArgs&&)
	{
		if(const auto pCon = FetchContainerPtr(*this))
			Close(*pCon);
	};
}

Rect
CloseButton::Refresh(const PaintContext& pc)
{
	auto r(Thumb::Refresh(pc));

	//画叉。
	{
		Size s(GetSizeOf(*this));

		if(s.Width > 8 && s.Height > 8)
		{
			const SPos xmin(pc.Location.X + 4), xmax(xmin + s.Width - 8),
				ymin(pc.Location.Y + 4), ymax(ymin + s.Height - 8);
			const Color c(IsEnabled(*this) ? ForeColor
				: FetchGUIShell().Colors[Styles::Workspace]);

			DrawLineSeg(pc.Target, xmin, ymin, xmax, ymax, c),
			DrawLineSeg(pc.Target, xmax - 1, ymin, xmin - 1, ymax, c);
		}
	}
	return r;
}

YSL_END_NAMESPACE(Components)

YSL_END

