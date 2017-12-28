/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
*
* Platform-dependent type definitions
*
* For the latest on this and related APIs, please see: https://github.com/Microsoft/cpprestsdk
*
* =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
****/

#pragma once

#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include "cpprest/details/cpprest_compat.h"

#ifndef _WIN32
# define __STDC_LIMIT_MACROS
# include <stdint.h>
#else
#include <cstdint>
#endif

#include "cpprest/details/SafeInt3.hpp"

// TEMPORARY, REMOVE:
#define _WIN32_USE_UTF8

#if defined(_WIN32) && !defined(_WIN32_USE_UTF8)
#define _UTF16_STRINGS
#endif

typedef char utf8char;
typedef std::string utf8string;
typedef std::stringstream utf8stringstream;
typedef std::ostringstream utf8ostringstream;
typedef std::ofstream utf8ofstream;
typedef std::ostream utf8ostream;
typedef std::istream utf8istream;
typedef std::ifstream utf8ifstream;
typedef std::istringstream utf8istringstream;

#ifdef _WIN32
typedef wchar_t utf16char;
typedef std::wstring utf16string;
typedef std::wstringstream utf16stringstream;
typedef std::wostringstream utf16ostringstream;
typedef std::wofstream utf16ofstream;
typedef std::wostream utf16ostream;
typedef std::wistream utf16istream;
typedef std::wifstream utf16ifstream;
typedef std::wistringstream utf16istringstream;
#else
typedef char16_t utf16char;
typedef std::u16string utf16string;
typedef std::basic_stringstream<utf16char> utf16stringstream;
typedef std::basic_ostringstream<utf16char> utf16ostringstream;
typedef std::basic_ofstream<utf16char> utf16ofstream;
typedef std::basic_ostream<utf16char> utf16ostream;
typedef std::basic_istream<utf16char> utf16istream;
typedef std::basic_ifstream<utf16char> utf16ifstream;
typedef std::basic_istringstream<utf16char> utf16istringstream;
#endif

namespace utility
{

// We should be using a 64-bit size type for most situations that do
// not involve specifying the size of a memory allocation or buffer.
typedef uint64_t size64_t;

#ifndef _WIN32
typedef uint32_t HRESULT; // Needed for PPLX
#endif

#ifdef _UTF16_STRINGS
//
// On Windows, all strings are wide
//
typedef utf16char char_t;
typedef utf16string string_t;
#define _XPLATSTR(x) L ## x
typedef utf16ostringstream ostringstream_t;
typedef utf16ofstream ofstream_t;
typedef utf16ostream ostream_t;
typedef utf16istream istream_t;
typedef utf16ifstream ifstream_t;
typedef utf16istringstream istringstream_t;
typedef utf16stringstream stringstream_t;
#define ucout std::wcout
#define ucin std::wcin
#define ucerr std::wcerr
#define uto_string std::to_wstring
#define uprintf wprintf
#ifdef _WIN32
#define usscanf_s swscanf_s
#define u_i64toa_s _i64tow_s
#define u_ui64toa_s _ui64tow_s
#define ustrnlen_s wcsnlen_s
#define u_sprintf_s_l _swprintf_s_l
#define u_stricmp _wcsicmp
#define u_dupenv_s _wdupenv_s
#endif
#else
//
// On POSIX platforms, all strings are narrow
//
typedef utf8char char_t;
typedef utf8string string_t;
#define _XPLATSTR(x) x
typedef utf8ostringstream ostringstream_t;
typedef utf8ofstream ofstream_t;
typedef utf8ostream ostream_t;
typedef utf8istream istream_t;
typedef utf8ifstream ifstream_t;
typedef utf8istringstream istringstream_t;
typedef utf8stringstream stringstream_t;
#define ucout std::cout
#define ucin std::cin
#define ucerr std::cerr
#define uto_string std::to_string
#define uprintf printf
#ifdef _WIN32
#define usscanf_s sscanf_s
#define u_i64toa_s _i64toa_s
#define u_ui64toa_s _ui64toa_s
#define ustrnlen_s strnlen_s
#define u_sprintf_s_l _sprintf_s_l
#define u_stricmp _stricmp
#define u_dupenv_s _dupenv_s
#endif
#endif // endif _UTF16_STRINGS

#ifndef _TURN_OFF_PLATFORM_STRING
#define U(x) _XPLATSTR(x)
#endif // !_TURN_OFF_PLATFORM_STRING

}// namespace utility


#if defined(_WIN32)
// Include on everything except Windows Desktop ARM, unless explicitly excluded.
#if !defined(CPPREST_EXCLUDE_WEBSOCKETS)
#if defined(WINAPI_FAMILY)
#if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP) && defined(_M_ARM)
#define CPPREST_EXCLUDE_WEBSOCKETS
#endif
#else
#if defined(_M_ARM)
#define CPPREST_EXCLUDE_WEBSOCKETS
#endif
#endif
#endif
#endif
