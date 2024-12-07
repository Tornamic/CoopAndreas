#ifndef INICPP_DLL_H
#define INICPP_DLL_H


#ifdef INICPP_DLL
#ifdef INICPP_EXPORT
#define INICPP_API __declspec(dllexport)
#else
#define INICPP_API __declspec(dllimport)
#endif
#else
#define INICPP_API
#endif


// Disable unwanted and not necessary MSVC++ warnings
#ifdef _MSC_VER
#pragma warning(disable : 4800)
#pragma warning(disable : 4251)
#endif


#endif // INICPP_DLL_H
