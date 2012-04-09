/*
	Copyright (C) by Franksoft 2009 - 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file GBKEX.cpp
\ingroup YReader
\brief �����ļ���
\version r3109; *Build 299 rev 4;
\author FrankHB<frankhb1989@gmail.com>
\since ���� build 132 ��
\par ����ʱ��:
	2009-12-09 06:28:06 +0800;
\par �޸�ʱ��:
	2012-04-06 07:27 +0800;
\par �ı�����:
	ANSI / GBK;
\par ģ������:
	YReader::GBKEX;
\deprecated ��ʱ������;��
*/


#include <ydef.h>
#include <Shells.h>

// GBK coded definitions:

namespace YSLib
{

#ifdef YCL_DS
#	define ROOTW
	const_path_t DEF_DIRECTORY = ROOTW "/Data/";
	//const char* DEF_FONT_NAME = ROOTW "����Ҧ��";
	//const_path_t DEF_FONT_PATH = ROOTW "/Font/FZYTK.TTF";
	const_path_t DEF_FONT_PATH = ROOTW "/Font/FZYTK.TTF";
	const_path_t DEF_FONT_DIRECTORY = ROOTW "/Font/";
#else
#	define ROOTW "H:\\NDS\\EFSRoot"
	const_path_t DEF_DIRECTORY = ROOTW "\\Data\\";
	//const char* DEF_FONT_NAME = "����Ҧ��";
	//const_path_t DEF_FONT_PATH = ROOTW "\\Font\\FZYTK.TTF";
	const_path_t DEF_FONT_PATH = ROOTW "\\Font\\FZYTK.TTF";
	const_path_t DEF_FONT_DIRECTORY = ROOTW "\\Font\\";
#endif

	/*
	using namespace Components;
	*/
}

//char gstr[128] = "������\t��= =ijkAB DE����i:�ߤީ�IF2\t3Xsk\nw\vwwww";

