﻿/*
	Copyright (C) by Franksoft 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ColorPicker.h
\ingroup YReader
\brief 拾色器。
\version r1101;
\author FrankHB<frankhb1989@gmail.com>
\since build 275 。
\par 创建时间:
	2012-01-06 20:46:10 +0800;
\par 修改时间:
	2012-01-13 00:34 +0800;
\par 文本编码:
	UTF-8;
\par 模块名称:
	YReader::ColorPicker;
*/


#ifndef INCLUDED_COLORPICKER_H_
#define INCLUDED_COLORPICKER_H_

#include <YSLib/UI/uicontx.h>
#include <YSLib/UI/scroll.h>

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

/*!
\brief 拾色框。
\since build 275 。
*/
class ColorBox : public DialogPanel
{
protected:
	/*!
	\brief 颜色区域。
	\since build 276 。
	*/
	Control ColorArea;
	HorizontalTrack RTrack, GTrack, BTrack;

public:
	ColorBox(const Point&, Color c = ColorSpace::White);

	DefGetter(const ynothrow, Color, Color, ColorArea.BackColor)
	void
	SetColor(Color);
};

YSL_END_NAMESPACE(Components)

YSL_END

#endif

