﻿/*
	© 2011-2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file FileSystem.cpp
\ingroup YCLib
\brief 平台相关的文件系统接口。
\version r2976
\author FrankHB <frankhb1989@gmail.com>
\since build 312
\par 创建时间:
	2012-05-30 22:41:35 +0800
\par 修改时间:
	2015-10-03 15:35 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib::FileSystem
*/


#include "YCLib/YModules.h"
#include YFM_YCLib_FileSystem // for std::accumulate, ystdex::read_uint_le,
//	std::min, ystdex::write_uint_le;
#include YFM_YCLib_NativeAPI // for Mode, struct ::stat, ::lstat;
#include YFM_YCLib_FileIO // for ystdex::throw_error, std::errc::not_supported,
//	FileOperationFailure, std::strchr, std::wctob, std::towupper, std::min,
//	ystdex::ntctsicmp, std::errc::invalid_argument;
#include "CHRLib/YModules.h"
#include YFM_CHRLib_CharacterProcessing // for CHRLib::MakeMBCS;
#if YCL_Win32
#	include YFM_Win32_YCLib_MinGW32 // for platform_ex::MakeFile;
#	include <time.h> // for ::localtime_s;

//! \since build 549
using platform_ex::DirectoryFindData;
#elif YCL_API_POSIXFileSystem
#	include <dirent.h>
#	include <time.h> // for ::localtime_r;

//! \since build 475
using namespace CHRLib;
#endif

namespace platform
{

#if !YCL_Win32
namespace
{

//! \since build 560
template<typename _type>
NodeCategory
FetchNodeCategoryFromStat(_type& st)
{
	auto res(NodeCategory::Empty);
	const auto m(Mode(st.st_mode) & Mode::FileType);

	if((m & Mode::Directory) == Mode::Directory)
		res |= NodeCategory::Directory;
	if((m & Mode::Link) == Mode::Link)
		res |= NodeCategory::Link;
	if((m & Mode::Regular) == Mode::Regular)
		res |= NodeCategory::Regular;
	if(YB_UNLIKELY((m & Mode::Character) == Mode::Character))
		res |= NodeCategory::Character;
	else if(YB_UNLIKELY((m & Mode::Block) == Mode::Block))
		res |= NodeCategory::Block;
	if(YB_UNLIKELY((m & Mode::FIFO) == Mode::FIFO))
		res |= NodeCategory::FIFO;
	if((m & Mode::Socket) == Mode::Socket)
		res |= NodeCategory::Socket;
	return res;
}

} // unnamed namespace;
#endif


void
CreateHardLink(const char* dst, const char* src)
{
#if YCL_Win32
	using namespace platform_ex;

	CreateHardLink(ucast(UTF8ToWCS(dst).c_str()),
		ucast(UTF8ToWCS(src).c_str()));
#elif YCL_DS
	yunused(dst), yunused(src);
	ystdex::throw_error(std::errc::not_supported);
#else
	if(::link(Nonnull(dst), Nonnull(src)) != 0)
		ystdex::throw_error(errno);
#endif
}
void
CreateHardLink(const char16_t* dst, const char16_t* src)
{
	// TODO: To make the behavior specific and same as on platform %Win32, use
	//	%::realpath on platform %Linux, etc.
#if YCL_Win32
	YCL_CallWin32F(CreateHardLinkW, wcast(dst), wcast(src), {});
#elif YCL_DS
	yunused(dst), yunused(src);
	ystdex::throw_error(std::errc::not_supported);
#else
	CreateHardLink(MakeMBCS(dst).c_str(), MakeMBCS(src).c_str());
#endif
}


DirectorySession::DirectorySession(const char* path)
#if YCL_Win32
	: dir(new DirectoryFindData(path ? path : ""))
#else
	: sDirPath(path && *path != char() ? path : "."),
	dir(::opendir(sDirPath.c_str()))
#endif
{
#if !YCL_Win32
	if(!dir)
		ystdex::throw_error<FileOperationFailure>(errno,
			"Opening directory failed.");
	ystdex::rtrim(sDirPath, YCL_PATH_DELIMITER);
	YAssert(sDirPath.empty() || sDirPath.back() != YCL_PATH_DELIMITER,
		"Invalid directory name state found.");
	sDirPath += YCL_PATH_DELIMITER;
#endif
}
DirectorySession::~DirectorySession()
{
#if !YCL_Win32
	const auto res(::closedir(dir));

	YAssert(res == 0, "No valid directory found.");
	yunused(res);
#else
	delete static_cast<DirectoryFindData*>(dir);
#endif
}

void
DirectorySession::Rewind() ynothrow
{
	YAssert(dir, "Invalid native handle found.");
#if YCL_Win32
	static_cast<DirectoryFindData*>(dir)->Rewind();
#else
	::rewinddir(dir);
#endif
}


HDirectory&
HDirectory::operator++()
{
	YAssert(!p_dirent || bool(GetNativeHandle()), "Invariant violation found.");
#if YCL_Win32
	p_dirent = static_cast<DirectoryFindData*>(GetNativeHandle())->Read();
#else
	p_dirent = ::readdir(GetNativeHandle());
#endif
	return *this;
}

NodeCategory
HDirectory::GetNodeCategory() const ynothrow
{
	if(p_dirent)
	{
		YAssert(bool(GetNativeHandle()), "Invariant violation found.");

		NodeCategory res(NodeCategory::Empty);
#if YCL_Win32
		using namespace platform_ex;
		const auto&
			dir_data(Deref(static_cast<DirectoryFindData*>(GetNativeHandle())));
		const auto& find_data(dir_data.GetFindData());
		const auto attr(FileAttributes(find_data.dwFileAttributes));

		if(attr & FileAttributes::Directory)
			res |= NodeCategory::Directory;
		if(attr & FileAttributes::ReparsePoint)
		{
			switch(find_data.dwReserved0)
			{
			case IO_REPARSE_TAG_SYMLINK:
				res |= NodeCategory::SymbolicLink;
				break;
			case IO_REPARSE_TAG_MOUNT_POINT:
				res |= NodeCategory::MountPoint;
			default:
				// TODO: Implement for other tags?
				;
			}
		}

		auto name(dir_data.GetDirName());

		YAssert(!name.empty() && name.back() == L'*', "Invalid state found.");
		name.pop_back();
		YAssert(!name.empty() && name.back() == L'\\', "Invalid state found.");
		// NOTE: Only existed and accessable files are considered.
		// FIXME: Blocked. TOCTTOU access.
		if(const auto h = MakeFile((name + Deref(static_cast<wstring*>(
			p_dirent))).c_str(), FileSpecificAccessRights::ReadAttributes,
			FileAttributesAndFlags::NormalWithDirectory))
			switch(::GetFileType(h.get())
				& ~static_cast<unsigned long>(FILE_TYPE_REMOTE))
			{
			case FILE_TYPE_CHAR:
				res |= NodeCategory::Character;
				break;
			case FILE_TYPE_PIPE:
				res |= NodeCategory::FIFO;
				break;
			default:
				;
			}
#else
		auto name(sDirPath + Deref(p_dirent).d_name);
		struct ::stat st;

#	if YCL_DS
		if(::stat(name.c_str(), &st) == 0)
#	else
		// TODO: Set error properly.
		if(::lstat(name.c_str(), &st) == 0)
			res |= FetchNodeCategoryFromStat(st);
		if(bool(res & NodeCategory::Link) && ::stat(name.c_str(), &st) == 0)
#	endif
			res |= FetchNodeCategoryFromStat(st);
#endif
		return res != NodeCategory::Empty ? res : NodeCategory::Invalid;
	}
	return NodeCategory::Empty;
}

const char*
HDirectory::GetName() const ynothrow
{
	if(p_dirent)
	{
#if !YCL_Win32
		return p_dirent->d_name;
#else
		// NOTE: See %YCL_Impl_RetTryCatchAll in 'fileSystem.cpp'.
		// XXX: Catch %std::bad_alloc?
		TryRet(utf8_name = platform_ex::WCSToUTF8(
			Deref(static_cast<wstring*>(p_dirent))), utf8_name.c_str())
		CatchIgnore(...)
#endif
	}
	return ".";
}


bool
IsAbsolute(const char* path)
{
#if YCL_Win32
	return Deref(path) == YCL_PATH_DELIMITER
		|| (*path != char() && path[1] == ':');
#else
	if(Deref(path) == '/')
		return true;

	const auto p(std::strstr(path, ":/"));

	return p && p != path && !std::strstr(p, ":/");
#endif
}
bool
IsAbsolute(const char16_t* path)
{
#if YCL_Win32
	return Deref(path) == u'\\' || (*path != char16_t() && path[1] == u':');
#else
	if(Deref(path) == u'/')
		return true;

	const u16string upath(path);
	const auto n(upath.find(u":/"));

	// TODO: Optimize for performance.
	return n != u16string::npos && n != 0
		&& upath.substr(n).find(u":/") == u16string::npos;
#endif
}

size_t
GetRootNameLength(const char* path)
{
	const char* p(std::strchr(Nonnull(path), ':'));

	return !p ? 0 : size_t(p - path + 1);
}


namespace FAT
{

namespace LFN
{

tuple<string, string, bool>
ConvertToAlias(const u16string& long_name)
{
	YAssert(ystdex::ntctslen(long_name.c_str()) == long_name.length(),
		"Invalid long file name found.");

	string alias;
	// NOTE: Strip leading periods.
	size_t offset(long_name.find_first_not_of(u'.'));
	// NOTE: Set when the alias had to be modified to be valid.
	bool lossy(offset != 0);
	const auto check_char([&](string& str, char16_t uc){
		int bc(std::wctob(std::towupper(wint_t(uc))));

		if(!lossy && std::wctob(wint_t(uc)) != bc)
			lossy = true;
		if(bc == ' ')
		{
			// NOTE: Skip spaces in filename.
			lossy = true;
			return;
		}
		// TODO: Optimize.
		if(bc == EOF || string(IllegalAliasCharacters).find(char(bc))
			!= string::npos)
			// NOTE: Replace unconvertible or illegal characters with
			//	underscores. See Microsoft FAT specification Section 7.4.
			yunseq(bc = '_', lossy = true);
		str += char(bc);
	});
	const auto len(long_name.length());

	for(; alias.length() < MaxAliasMainPartLength && long_name[offset] != u'.'
		&& offset != len; ++offset)
		check_char(alias, long_name[offset]);
	if(!lossy && long_name[offset] != u'.' && long_name[offset] != char16_t())
		// NOTE: More than 8 characters in name.
		lossy = true;

	auto ext_pos(long_name.rfind(u'.'));
	string ext;

	if(!lossy && ext_pos != u16string::npos && long_name.rfind(u'.', ext_pos)
		!= u16string::npos)
		// NOTE: More than one period in name.
		lossy = true;
	if(ext_pos != u16string::npos && ext_pos + 1 != len)
	{
		++ext_pos;
		for(size_t ext_len(0); ext_len < LFN::MaxAliasExtensionLength
			&& ext_pos != len; yunseq(++ext_len, ++ext_pos))
			check_char(ext, long_name[ext_pos]);
		if(ext_pos != len)
			// NOTE: More than 3 characters in extension.
			lossy = true;
	}
	return make_tuple(std::move(alias), std::move(ext), lossy);
}

string
ConvertToMBCS(const char16_t* path)
{
	// TODO: Optimize?
	ImplRet(ystdex::restrict_length(CHRLib::MakeMBCS({path,
		std::min<size_t>(ystdex::ntctslen(path), MaxLength)}), MaxMBCSLength))
}

EntryDataUnit
GenerateAliasChecksum(const EntryDataUnit* p) ynothrowv
{
	static_assert(std::is_same<EntryDataUnit, unsigned char>::value,
		"Only unsigned char as byte is supported by checksum generation.");

	YAssertNonnull(p);
	// NOTE: The operation is an unsigned char rotate right.
	return std::accumulate(p, p + AliasEntryLength, 0,
		[](EntryDataUnit v, EntryDataUnit b){
			return ((v & 1) != 0 ? 0x80 : 0) + (v >> 1) + b;
		});
}

YF_API void
WriteNumericTail(string& alias, size_t k) ynothrowv
{
	YAssert(!(MaxAliasMainPartLength < alias.length()), "Invalid alias found.");

	auto p(&alias[MaxAliasMainPartLength - 1]);

	while(k > 0)
	{
		*p-- = '0' + k % 10;
		k /= 10;
	}
	*p = '~';
}

} // namespace LFN;

//! \since build 607
namespace
{

yconstexpr bool
is_time_no_leap_valid(const std::tm& t)
{
	return !(t.tm_hour < 0 || 23 < t.tm_hour || t.tm_hour < 0 || 59 < t.tm_min
		|| t.tm_sec < 0 || 59 < t.tm_min);
}
yconstexpr bool
is_date_range_valid(const std::tm& t)
{
	return !(t.tm_mon < 0 || 12 < t.tm_mon || t.tm_mday < 1 || 31 < t.tm_mday);
}

} // unnamed namespace;

std::time_t
ConvertFileTime(Timestamp d, Timestamp t) ynothrow
{
	struct std::tm time_parts;

	yunseq(
	time_parts.tm_hour = t >> 11,
	time_parts.tm_min = (t >> 5) & 0x3F,
	time_parts.tm_sec = (t & 0x1F) << 1,
	time_parts.tm_mday = d & 0x1F,
	time_parts.tm_mon = ((d >> 5) & 0x0F) - 1,
	time_parts.tm_year = (d >> 9) + 80,
	time_parts.tm_isdst = 0
	);
	return std::mktime(&time_parts);
}

pair<Timestamp, Timestamp>
FetchDateTime() ynothrow
{
	struct std::tm tmp;
	std::time_t epoch;

	if(std::time(&epoch) != std::time_t(-1))
	{
#if YCL_Win32
		// NOTE: The return type and parameter order differs than ISO C11
		//	library extension.
		::localtime_s(&tmp, &epoch);
#else
		::localtime_r(&epoch, &tmp);
		// FIXME: For platforms without %::(localtime_r, localtime_s).
#endif
		// NOTE: Microsoft FAT base year is 1980.
		return {is_date_range_valid(tmp) ? ((tmp.tm_year - 80) & 0x7F) << 9
			| ((tmp.tm_mon + 1) & 0xF) << 5 | (tmp.tm_mday & 0x1F) : 0,
			is_time_no_leap_valid(tmp) ? (tmp.tm_hour & 0x1F) << 11
			| (tmp.tm_min & 0x3F) << 5 | ((tmp.tm_sec >> 1) & 0x1F) : 0};
	}
	return {0, 0};
}


void
EntryData::CopyLFN(char16_t* str) const ynothrowv
{
	const auto pos(LFN::FetchLongNameOffset((*this)[LFN::Ordinal]));

	YAssertNonnull(str);
	for(size_t i(0); i < LFN::EntryLength; ++i)
		if(pos + i < LFN::MaxLength - 1)
			str[pos + i]
				= ystdex::read_uint_le<16>(data() + LFN::OffsetTable[i]);
}

bool
EntryData::FindAlias(const char* name, size_t len) const
{
	const auto alias(GenerateAlias());

	return ystdex::ntctsicmp(name, alias.c_str(),
		std::min<size_t>(alias.length(), len)) == 0;
}

string
EntryData::GenerateAlias() const
{
	if((*this)[0] != Free)
	{
		if((*this)[0] == '.')
			return (*this)[1] == '.' ? ".." : ".";

		// NOTE: Copy the base filename.
		bool case_info(((*this)[CaseInfo] & LFN::CaseLowerBasename) != 0);
		const auto conv([&](size_t i){
			const auto c((*this)[i]);

			return char(case_info ? std::tolower(int(c)) : c);
		});
		string res;

		res.reserve(LFN::MaxAliasLength - 1);
		for(size_t i(0); i < LFN::MaxAliasMainPartLength
			&& (*this)[Name + i] != ' '; ++i)
			res += conv(Name + i);
		if((*this)[Extension] != ' ')
		{
			res += '.';
			case_info = ((*this)[CaseInfo] & LFN::CaseLowerExtension) != 0;
			for(size_t i(0); i < LFN::MaxAliasExtensionLength
				&& (*this)[Extension + i] != ' '; ++i)
				res += conv(Extension + i);
		}
		return res;
	}
	return {};
}

void
EntryData::SetupRoot(ClusterIndex root_cluster) ynothrow
{
	Clear();
	ClearAlias(),
	SetDot(Name),
	SetDirectoryAttribute();
	WriteCluster(root_cluster);
}

void
EntryData::WriteCluster(ClusterIndex c) ynothrow
{
	using ystdex::write_uint_le;

	write_uint_le<16>(data() + Cluster, c),
	write_uint_le<16>(data() + ClusterHigh, c >> 16);
}

void
EntryData::WriteAlias(const string& alias) ynothrow
{
	size_t i(0), j(0);

	for(; j < LFN::MaxAliasMainPartLength && alias[i] != '.'
		&& alias[i] != char(); yunseq(++i, ++j))
		(*this)[j] = EntryDataUnit(alias[i]);
	while(j < LFN::MaxAliasMainPartLength)
	{
		(*this)[j] = ' ';
		++j;
	}
	if(alias[i] == '.')
		for(++i; alias[i] != char() && j < LFN::AliasEntryLength;
			yunseq(++i, ++j))
			(*this)[j] = EntryDataUnit(alias[i]);
	for(; j < LFN::AliasEntryLength; ++j)
		(*this)[j] = ' ';
}

void
EntryData::WriteCDateTime() ynothrow
{
	using ystdex::write_uint_le;
	const auto& date_time(FetchDateTime());

	write_uint_le<16>(data() + CTime, date_time.second),
	write_uint_le<16>(data() + CDate, date_time.first);
}

void
EntryData::WriteDateTime() ynothrow
{
	using ystdex::write_uint_le;
	using ystdex::unseq_apply;
	const auto date_time(FetchDateTime());
	const auto dst(data());

	unseq_apply([&](size_t offset){
		write_uint_le<16>(dst + offset, date_time.first);
	}, CDate, MDate, ADate),
	unseq_apply([&](size_t offset){
		write_uint_le<16>(dst + offset, date_time.second);
	}, CTime, MTime);
}

const char*
CheckColons(const char* path)
{
	if(const auto p_col = std::strchr(Nonnull(path), ':'))
	{
		path = p_col + 1;
		if(std::strchr(path, ':'))
			ystdex::throw_error(std::errc::invalid_argument);
	}
	return path;
}

} // namespace FAT;

} // namespace platform;

namespace platform_ex
{

#if !YCL_Win32
char16_t
FS_IsRoot(const char16_t* str)
{
	const u16string ustr(str);

	return ustr == u"/"
#	if YCL_DS
		|| ustr == u"fat:/" || ustr == u"sd:/"
#	endif
	;
}
#endif

} // namespace platform_ex;

