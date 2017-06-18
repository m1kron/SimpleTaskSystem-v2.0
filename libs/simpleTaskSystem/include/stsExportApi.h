#pragma once

#if defined( STS_DLL )
#define STS_API __declspec(dllexport) 
#else
#define STS_API __declspec(dllimport) 
#endif