﻿/*
	Copyright (C) by Franksoft 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file listbox.h
\ingroup Shell
\brief 样式相关的图形用户界面列表框控件。
\version 0.3148;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2011-03-07 20:30:40 +0800;
\par 修改时间:
	2011-04-26 16:05 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Shell::ListBox;
*/


#ifndef YSL_INC_SHELL_LISTBOX_H_
#define YSL_INC_SHELL_LISTBOX_H_

#include "ymenu.h"
#include "scroll.h"
//#include "ystyle.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

YSL_BEGIN_NAMESPACE(Widgets)

YSL_END_NAMESPACE(Widgets)

YSL_BEGIN_NAMESPACE(Controls)

/*!
\brief 文本列表框。

带滚动条的文本列表框。
*/
class ListBox : public ScrollableContainer
{
public:
	typedef Menu::ListType ListType;
	typedef Menu::ViewerType ViewerType;

private:
	Menu TextListBox; //文本列表框。

public:
	explicit
	ListBox(const Rect& = Rect::Empty, GHandle<ListType> = nullptr);

	DefPredicateMember(Selected, TextListBox)
	PDefH1(bool, Contains, ViewerType::IndexType i)
		ImplBodyMember1(TextListBox, Contains, i)

	/*!
	\brief 取顶端控件指针。
	*/
	virtual IControl*
	GetTopControlPtr(const Point&);
	DefGetterMember(ViewerType::IndexType, HeadIndex, TextListBox)
	DefGetterMember(ViewerType::IndexType, SelectedIndex, TextListBox)
	DefGetterMember(ListType&, List, TextListBox)
	DefMutableDepEventGetterMember(HVisualEvent, ViewChanged, TextListBox) \
		//!< 视图变更事件。
	DefMutableDepEventGetterMember(HIndexEvent, Selected, TextListBox) \
		//!< 项目选择状态变更事件。
	DefMutableDepEventGetterMember(HIndexEvent, Confirmed, TextListBox) \
		//!< 项目选中确定事件。

	/*!
	\brief 绘制界面。
	*/
	virtual void
	Paint();

	PDefH0(void, ResetView)
		ImplBodyMember0(TextListBox, ResetView)
	PDefH0(void, UpdateView)
		ImplBodyMember0(TextListBox, UpdateView)

private:
	/*!
	\brief 处理垂直滚动条滚动事件。
	*/
	void
	OnScroll_VerticalScrollBar(ScrollEventArgs&);

	/*!
	\brief 处理文本列表框视图变更事件。
	*/
	void
	OnViewChanged_TextListBox(EventArgs&);
};


//! \brief 文件列表框。
class FileBox : public IO::FileList, public ListBox
{
public:
	explicit
	FileBox(const Rect& = Rect::Empty);

	/*!
	\brief 取当前路径。
	*/
	IO::Path
	GetPath() const;

private:
	/*!
	\brief 处理确认事件。
	*/
	void
	OnConfirmed(IndexEventArgs&);
};

YSL_END_NAMESPACE(Controls)

YSL_END_NAMESPACE(Components)

YSL_END

#endif
