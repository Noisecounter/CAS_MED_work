// 67d7842dbbe25473c3c32b93c0da8047785f30d78e8a024de1b57352245f9689

#ifndef _UTILS_TLM_CMNHDR_H_
#define _UTILS_TLM_CMNHDR_H_

#ifdef _MSC_VER
#define TLM_EXPORT __declspec( dllexport )
#define TLM_IMPORT __declspec( dllimport )
#else
#define TLM_EXPORT
#define TLM_IMPORT
#endif

// work around the c++14 ::gets mess
#if defined(__cplusplus) && __cplusplus>201103L
#include <cstdio>
#if defined(_MSC_VER) || defined(__MINGW32__) || defined(__MINGW64__)
// stdio properly undefines gets for c++14 : fake it here
namespace std { extern char* gets( char* ); }
#else
#if __GLIBC_PREREQ(2,15) && defined(_GNU_SOURCE)
// stdio properly undefines gets for c++14 : fake it here
namespace std { extern char* gets( char* ); }
#else
// stdio improperly edfines gets, use it...
namespace std { using ::gets; }
#endif
#endif
#endif
//

/* Includes */
#include <systemc.h>

//using sc_core::SC_ID_ASSERTION_FAILED_;
#include <tlm.h>

#endif /* _UTILS_TLM_CMNHDR_H_ */


