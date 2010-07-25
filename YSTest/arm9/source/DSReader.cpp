﻿// YReader -> DSReader by Franksoft 2010
// CodePage = UTF-8;
// CTime = 2010-1-5 14:04:05;
// UTime = 2010-7-25 10:28;
// Version = 0.2558;


#include "DSReader.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(DS)

using namespace Drawing;
using namespace Exceptions;
using namespace Text;

YSL_BEGIN_NAMESPACE(Components)

u32 MDualScreenReader::TextFill(u32 off)
{
	const uchar_t* s = Blocks[0].GetPtr() + off;
	s += trUp.PutString(s);
	return s + trDn.PutString(s) - off - Blocks[0].GetPtr();
}

MDualScreenReader::MDualScreenReader(YTextFile& tf_,
				   u16 l, u16 w, u16 t_up, u16 h_up, u16 t_down, u16 h_down,
				   YFontCache& fc_)
try : tf(tf_), Blocks(tf), fc(fc_),
left(l), top_up(t_up), top_down(t_down),
pBgUp(pDesktopUp->GetBackgroundPtr()), pBgDn(pDesktopDown->GetBackgroundPtr()),
trUp(*new YTextRegion(fc_)), trDn(*new YTextRegion(fc_)), rot(RDeg0),
nLoad(0), offUp(0), offDn(0)
{
	trUp.SetSize(w, h_up);
	trDn.SetSize(w, h_down);
	SetFontSize();
	SetColor();
	SetLineGap();
	Reset();
	if(!tf.IsValid())
		trUp.PutString(L"文件打开失败！\n");
	nLoad = Blocks[0].Load(tf);
}
catch(YLoggedError&)
{
	throw;
}
catch(...)
{
	throw YLoggedError("Error occured @@ MDualScreenReader::MDualScreenReader();");
}
MDualScreenReader::~MDualScreenReader()
{
	delete &trUp;
	delete &trDn;
}

bool
MDualScreenReader::IsTextTop()
{
	return !offUp;
}
bool
MDualScreenReader::IsTextBottom()
{
	return false; //Temp;
}

void
MDualScreenReader::SetColor(PixelType c)
{
	trUp.SetColor(c);
	trDn.SetColor(c);
}
void
MDualScreenReader::SetLineGap(u8 g)
{
	trUp.SetLineGap(g);
	trDn.SetLineGap(g);
}
void
MDualScreenReader::SetFontSize(MFont::SizeType fz)
{
	fc.SetFontSize(fz);	
	lnHeight = fc.GetHeight();
}
/*
void MDualScreenReader::SetLnNNow(u8 n)
{
	if(n >= 0)
		trUp.SetLnNNow(n);
}
*/

void
MDualScreenReader::Reset()
{
	Clear();
	ResetPen();
}
void
MDualScreenReader::PrintText()
{
//	trUp.BlitToBuffer(pBgUp, RDeg0, SCRW, SCRH, 0, 0, 0, 0, trUp.GetWidth(), trUp.GetBufferHeightResized());
//	trUp.BlitToBuffer(pBgUp, RDeg0);
	trUp.BlitToBuffer(pBgUp, rot);
	trDn.BlitToBuffer(pBgDn, rot);
}
void
MDualScreenReader::Refresh()
{
	InsertMessage(NowShell(), SM_DSR_REFRESH, 0xD2, reinterpret_cast<WPARAM>(this));
}
void
MDualScreenReader::TextInit()
{
	Reset();
	offDn = TextFill(offUp = 0);
}
void
MDualScreenReader::Update()
{
	Reset();
	offDn = offUp + TextFill(offUp);
}

bool
MDualScreenReader::LineUp()
{
	if(IsTextTop())
		return false;

	const u8 h = lnHeight, hx = h + GetLnGapDn();
	const u16 w = trUp.GetWidth();
	const u32 t = w * h, s = (trUp.GetHeight() - trUp.GetMarginResized() - h) * w, d = trDn.Margin.Top * w;

	trDn.Move(hx, trDn.GetBufferHeightResized());
	memcpy(&trDn.GetBufferPtr()[d], &trUp.GetBufferPtr()[s], t * sizeof(PixelType));
	memcpy(&trDn.GetBufferAlphaPtr()[d], &trUp.GetBufferAlphaPtr()[s], t * sizeof(u8));
	trUp.Move(hx, trUp.GetBufferHeightResized());
	trUp.ClearLn(0);
	trUp.SetLnNNow(0);
	trUp.PutLine(&Blocks[0].GetPtr()[offUp = trUp.GetPrevLnOff(Blocks[0].GetPtr(), offUp)]);
	offDn = trDn.GetPrevLnOff(Blocks[0].GetPtr(), offDn);
	return true;
}
bool
MDualScreenReader::LineDown()
{
	if(IsTextBottom())
		return false;

	const u8 h = lnHeight, hx = h + GetLnGapUp();
	const u16 w = trUp.GetWidth();
	const u32 t = w * h, s = trUp.Margin.Top * w, d = (trUp.GetHeight() - trUp.GetMarginResized() - h) * w;

	trUp.Move(-hx);
	memcpy(&trUp.GetBufferPtr()[d], &trDn.GetBufferPtr()[s], t * sizeof(PixelType));
	memcpy(&trUp.GetBufferAlphaPtr()[d], &trDn.GetBufferAlphaPtr()[s], t * sizeof(u8));
	trDn.Move(-hx);
	trDn.ClearLnLast();
	trDn.SetLnLast();
	offDn += trDn.PutLine(&Blocks[0].GetPtr()[offDn]);
	offUp = trUp.GetNextLnOff(Blocks[0].GetPtr(), offUp);
	return true;
}

bool
MDualScreenReader::ScreenUp()
{
	if(IsTextTop())
		return false;
	offUp = trUp.GetPrevLnOff(Blocks[0].GetPtr(), offUp, trUp.GetLnN() + trDn.GetLnN());
	Update();
	return true;
}
bool
MDualScreenReader::ScreenDown()
{
	if(IsTextBottom())
		return false;
	offUp = offDn;
	Update();
	return true;
}

void
MDualScreenReader::Scroll(PFVOID pCheck)
{

}


void
YWndDSReader::DrawForeground()
{
}

YSL_END_NAMESPACE(Components)

YSL_END_NAMESPACE(DS)

YSL_END
