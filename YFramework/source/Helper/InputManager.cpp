﻿/*
	© 2012-2013 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file InputManager.cpp
\ingroup Helper
\brief 输入管理器。
\version r347
\author FrankHB <frankhb1989@gmail.com>
\since build 323
\par 创建时间:
	2012-07-06 11:23:21 +0800
\par 修改时间:
	2013-12-24 00:39 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	Helper::InputManager
*/


#include "Helper/YModules.h"
#include YFM_Helper_InputManager
#include YFM_YCLib_Input
#include YFM_Helper_Host
#include YFM_Helper_HostRenderer // for Host::Window, Host::RenderWindow;
#include YFM_Helper_GUIApplication // for FetchEnvironment;

namespace YSLib
{

using namespace UI;

namespace Devices
{

InputManager::InputManager()
	: GUI_state(FetchGUIState()), cursor_state()
#if YF_Hosted
	, env(Host::FetchEnvironment())
#endif
{}

#if YCL_DS
#	define YCL_KEY_Touch KeyCodes::Touch
#	define YCL_CURSOR_VALID
#elif YCL_Win32
#	define YCL_KEY_Touch VK_LBUTTON
#	define YCL_CURSOR_VALID if(cursor_state != Point::Invalid)
#else
#	error "Unsupported platform found."
#endif
void
InputManager::DispatchInput(IWidget& wgt)
{
	const auto disp([&](const KeyInput& keyset, VisualEvent key_evt,
		VisualEvent touch_evt){
#if YCL_Win32
		if(keyset[YCL_KEY_Touch] || keyset[VK_RBUTTON] || keyset[VK_MBUTTON])
#else
		if(keyset[YCL_KEY_Touch])
#endif
		{
			YCL_CURSOR_VALID
			{
				CursorEventArgs e(wgt, keyset, cursor_state);

				GUI_state.get().ResponseCursor(e, touch_evt);
			}
		}
		else if(keyset.any())
		{
			KeyEventArgs e(wgt, keyset);

			GUI_state.get().ResponseKey(e, key_evt);
		}
	});
	KeyInput keys(platform_ex::FetchKeyUpState());

	disp(keys, KeyUp, TouchUp);
#if YCL_Win32
	YCL_CURSOR_VALID
	{
		CursorEventArgs e(wgt, keys, cursor_state);

		GUI_state.get().ResponseCursor(e, CursorOver);
	}
#endif
	keys = platform_ex::FetchKeyDownState();
	disp(keys, KeyDown, TouchDown);
	keys = platform_ex::FetchKeyState();
	disp(keys, KeyHeld, TouchHeld);
#if YCL_Win32
	const UI::WheelDelta raw_mouse(env.get().RawMouseButton);

	if(raw_mouse != 0)
	{
		CursorWheelEventArgs e(wgt, raw_mouse, keys, cursor_state);

		GUI_state.get().ResponseCursor(e, CursorWheel);
		env.get().RawMouseButton = 0;
	}
#endif
}

IWidget*
InputManager::Update()
{
#if YCL_Win32
	const auto p_wnd(env.get().GetForegroundWindow());

	if(!p_wnd)
		return nullptr;

#endif
	using namespace platform::KeyCodes;

	// FIXME: [DS] crashing after sleeping(default behavior of closing then
	//	reopening lid) on real machine due to LibNDS default interrupt
	//	handler for power management.
//	platform::AllowSleep(true);
	platform_ex::UpdateKeyStates();

#if YCL_DS
	if(platform_ex::FetchKeyState()[YCL_KEY_Touch])
#endif
	{
		using namespace Drawing;

		platform::CursorInfo cursor;

		platform_ex::WriteCursor(cursor);
#if YCL_DS
		cursor_state = cursor.operator Point();
#elif YCL_Win32
		::ScreenToClient(p_wnd->GetNativeHandle(), &cursor);

		const auto& pr(p_wnd->GetInputBounds());

		if(YB_LIKELY(pr.first.X != pr.second.X && pr.first.Y != pr.second.Y)
			&& (!p_wnd->BoundsLimited
			|| (IsInInterval< ::LONG>(cursor.x, pr.first.X, pr.second.X)
			&& IsInInterval< ::LONG>(cursor.y, pr.first.Y, pr.second.Y))))
			yunseq(cursor_state.X = cursor.x - pr.first.X,
				cursor_state.Y = cursor.y - pr.first.Y);
		else
			cursor_state = Point::Invalid;
#endif
	}
#if YF_Hosted
	if(const auto p_render_wnd = dynamic_cast<Host::RenderWindow*>(p_wnd))
		return &p_render_wnd->GetRenderer().GetWidgetRef();
#endif
	return nullptr;
}
#undef YCL_CURSOR_VALID
#undef YCL_KEY_Touch

} // namespace Devices;

} // namespace YSLib;

