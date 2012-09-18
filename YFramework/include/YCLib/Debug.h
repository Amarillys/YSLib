﻿/*
	Copyright (C) by Franksoft 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Debug.h
\ingroup YCLib
\brief YCLib 调试设施。
\version r110
\author FrankHB<frankhb1989@gmail.com>
\since build 299
\par 创建时间:
	2012-04-07 14:20:49 +0800
\par 修改时间:
	2012-09-04 12:27 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib::Debug
*/


#ifndef YCL_INC_DEBUG_H_
#define YCL_INC_DEBUG_H_ 1

#include "ycommon.h"

/*!
\since build 314
*/
//@{
#ifndef NDEBUG
#	define YCL_DEBUG_PRINTF(...) std::printf(__VA_ARGS__)
#	define YCL_DEBUG_PUTS(_arg) std::puts(_arg)
#else
#	define YCL_DEBUG_PRINTF(...)
#	define YCL_DEBUG_PUTS(_arg)
#endif
//@}

namespace platform
{

/*!
\brief 调试模式：设置状态。
\note 当且仅当状态为 true 时，以下除 YDebugGetStatus 外的调试模式函数有效。
\warning 不保证线程安全性。
*/
void
YDebugSetStatus(bool = true);

/*!
\brief 调试模式：取得状态。
\warning 不保证线程安全性。
*/
bool
YDebugGetStatus();

/*!
\brief 调试模式：显示控制台。
\warning 控制台显示状态不保证线程安全性。
\since build 312
*/
void
YDebugBegin();

/*!
\brief 调试模式：按键继续。
\warning 控制台显示状态不保证线程安全性。
*/
void
YDebug();
/*!
\brief 调试模式：显示控制台字符串，按键继续。
\warning 控制台显示状态不保证线程安全性。
*/
void
YDebug(const char*);

/*!
\brief 调试模式 printf ：显示控制台格式化输出 ，按键继续。
\warning 控制台显示状态不保证线程安全性。
*/
#if defined _WIN32 && !defined __USE_MINGW_ANSI_STDIO
YB_ATTRIBUTE((format (ms_printf, 1, 2)))
#else
YB_ATTRIBUTE((format (printf, 1, 2)))
#endif
int
yprintf(const char*, ...);

} // namespace platform_ex;

#endif
