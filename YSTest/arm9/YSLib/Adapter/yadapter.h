// YSLib::Adapter::YAdapter by Franksoft 2010
// CodePage = UTF-8;
// CTime = 2010-2-22 20:16:21;
// UTime = 2010-7-9 10:11;
// Version = 0.1666;


#ifndef INCLUDED_YADAPTER_H_
#define INCLUDED_YADAPTER_H_

// YAdapter ���ⲿ�������

//�������á�
//#define NDEBUG
#define YC_USE_YASSERT

//����ƽ̨�ļ���
#include <platform.h>

namespace YSLib
{
	namespace Drawing
	{
		typedef ::PIXEL PixelType;
	}

	using ::iprintf;

	using ::swiWaitForVBlank;

	using ::lcdMainOnTop;
	using ::lcdMainOnBottom;
	using ::lcdSwap;
	using ::videoSetMode;
	using ::videoSetModeSub;

	using ::scanKeys;
	using ::touchRead;

	// LibFAT �������ͺͺ������塣
	using ::stat;
	using ::DIR_ITER;
	using ::diropen;
	using ::dirnext;
	using ::dirreset;
	using ::dirclose;
	using ::mkdir;
	using ::chdir;
	using ::getcwd;
}


//���� CHRLib ��

#include "../../CHRLib/chrlib.h"

namespace stdex
{
	using CHRLib::uchar_t;
}

namespace YSLib
{
	namespace Text
	{
		using namespace CHRLib;
	}
	using Text::uchar_t;
}


//���� YCLib ��

#include "../../YCLib/ycommon.h"

namespace YSLib
{
	namespace Drawing
	{
		using platform_type::BitmapPtr;
		using platform_type::ConstBitmapPtr;
	}
	namespace Runtime
	{
		using platform_type::Keys;
		using platform_type::KeysInfo;
		using platform_type::CursorInfo;
	}
	using platform::yassert;
	namespace DS
	{
		using namespace platform;
	}
}


//���� FreeType2 ��

#include <ft2build.h>

#include FT_FREETYPE_H
#include FT_CACHE_H
#include FT_BITMAP_H
#include FT_GLYPH_H
//#include FT_OUTLINE_H


//���� Anti-Grain Geometry ��
//#include "yagg.h"


//���� Loki ��
#include <loki/Function.h>
#include <loki/SmartPtr.h>
#include <loki/StrongPtr.h>

namespace YSLib
{
	namespace Design
	{
		//���Ͳ���������������
		using Loki::Typelist;
		namespace TL = Loki::TL;
		using Loki::NullType;
		using Loki::EmptyType;
		using Loki::Int2Type;
		using Loki::Type2Type;
		using Loki::Select;
		using Loki::IsCustomUnsignedInt;
		using Loki::IsCustomSignedInt;
		using Loki::IsCustomFloat;
		using Loki::TypeTraits;

		// Function �� Function ��
		using Loki::Function;
		using Loki::Functor;

		//���ģʽ��
		namespace Pattern
		{
		}
	}
	//ʹ�� Loki ����ָ�롣
	using Loki::SmartPtr;
	using Loki::RefCounted;
	using Loki::AllowConversion;
	using Loki::DisallowConversion;
	using Loki::RejectNull;
	using Loki::DefaultSPStorage;
	using Loki::HeapStorage;
	using Loki::RefToValue;
	using Loki::SmallObject;
}

//ʹ�� Loki �ı����ھ�̬��顣
#define YSL_STATIC_CHECK(expr, msg) LOKI_STATIC_CHECK(expr, msg)

//ʹ�� Loki �� Typelist ��
#define YSL_TL(n, ...) LOKI_TYPELIST##n(...)

//ʹ������ָ��ʵ�ֵľ����
//#define YSL_USE_SIMPLE_HANDLE
#define YSL_HANDLEPOLICY_SIMPLE


//�ಥ�¼���
#define YSL_EVENT_MULTICAST

#endif

