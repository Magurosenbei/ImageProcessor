#ifndef TERMS_H
#define TERMS_H

#define APP_TITLE       "XOpenGL"
#define APP_PROCESS     "XOpenGL"

#define APP_WIDTH       1024
#define APP_HEIGHT      768
#define APP_CHAR_CODEC "shift-jis"

#define PIQUARTER       0.785398162f
#define PIHALF	        1.570796325f
#define PI		        3.14159265f
#define PI2		        6.2831853f

#define DEGTORAD(a)         (a * 0.01745329f)
#define RADTODEG(a)         (a * 57.29577957f)
#define CLAMP(x, Min, Max)  (min(Max, max(x, Min)))
#define ROUND_UP(a, b)      ((a % b > 0)? ((a + b) & ~(b - 1)) : a)
#define ROUND_DOWN(a, b)    ((a % b > 0)? ((a + b) & ~(b - 1)) - b : a)

#define SQRT_MAGIC_F 0x5f3759df
#define SQRT_MAGIC_D 0x5fe6ec85e7de30da

//  Memory  Macro / Definition
#define SmartPtr        std::auto_ptr
#define NEW             ALIGN_NEW_CELL

#define ALIGN_NEW_CELL(Bytes, bits)         malloc_align(Bytes, bits)
#define ALIGN(Bytes)                        __attribute__((aligned(Bytes)))
#define ALIGN_NEW(Bytes, bits)              General_aligned_malloc(Bytes, 1 << bits)
#define ALIGN_NEW_BYTE(Bytes, bytesalign)   General_aligned_malloc(Bytes, bytesalign)
#define ALIGN_DEL(Addr)                     General_aligned_free(Addr)

//  Text Tools
#define MULTI_TEXT      _bstr_t
#define FORMATTEXT		System_Utility::FormatText

//  COLOR Bit Operations 4 Byte
#define ARGB(a,r,g,b)	((DWORD(a)<<24) + (DWORD(r)<<16) + (DWORD(g)<<8) + DWORD(b))
#define GETA(col)		((col)>>24)
#define GETR(col)		(((col)>>16) & 0xFF)
#define GETG(col)		(((col)>>8) & 0xFF)
#define GETB(col)		((col) & 0xFF)
#define SETA(col,a)		(((col) & 0x00FFFFFF) + (DWORD(a)<<24))
#define SETR(col,r)		(((col) & 0xFF00FFFF) + (DWORD(r)<<16))
#define SETG(col,g)		(((col) & 0xFFFF00FF) + (DWORD(g)<<8))
#define SETB(col,b)		(((col) & 0xFFFFFF00) + DWORD(b))

//  DEBUG USAGE
#ifdef _MSC_VER
#ifdef NDEBUG
#define ASSERT(exp, msg)	{}
#define VERIFY(exp, msg)	(exp);
#else // NDEBUG
#define ASSERT(exp, msg)	if( !(exp) ) std::_Debug_message(L#msg, _CRT_WIDE(__FILE__), __LINE__);
#define VERIFY(exp, msg)	if( !(exp) ) std::_Debug_message(L#msg, _CRT_WIDE(__FILE__), __LINE__);
#endif // end NDEBUG
#else // _MSC_VER
#ifdef NDEBUG
#define ASSERT(exp, msg)	{}
#define VERIFY(exp, msg)	(exp);
#else // NDEBUG
#include <cassert>
#define ASSERT(exp, msg)		assert( (exp) );
#define VERIFY(exp, msg)		assert( (exp) );
#endif // end NDEBUG
#endif // end _MSC_VER

#ifndef _DEBUG
#define WriteCommandLine printf
#else
#define WriteCommandLine(a, ...)	{}
#endif

#endif
