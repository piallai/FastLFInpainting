/******************************************************************/
/***            Pierre Allain, INRIA, February 2020				  */
/******************************************************************/


#pragma once
/*! This file contains convenient macros.*/


#if defined(_MSC_VER)
#define DEBUG_BREAK __debugbreak()
#elif defined(__GNUC__) || defined(__GNUG__)
#define DEBUG_BREAK __builtin_trap()
#else
#error Unknown compiler
#endif


#define COMMA ,
/*! Macro to conveniently display a variable's value. Mix of Display and Debug.*/
#define displug(arg) std::cout << #arg << " = " << arg << std::endl;

#if defined(CELF_SHARED_LIBRARIES) && defined(_WIN32)
//#define COMPILING_THE_DLL
#ifdef COMPILING_THE_DLL
#define EXTERN_CELF_API extern "C" __declspec(dllexport)
#else
#define EXTERN_CELF_API extern "C" __declspec(dllimport)
#endif
#else
#define EXTERN_CELF_API extern
#endif

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
