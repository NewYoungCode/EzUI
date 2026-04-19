#pragma once

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4996)
#endif

#include <fmt/format.h>

#ifdef _MSC_VER
#pragma warning(pop)
#endif


#ifdef _WIN64
#ifdef _DEBUG
#pragma comment(lib, "x64/fmtd.lib")
#else
#pragma comment(lib, "x64/fmt.lib")
#endif
#else
#ifdef _DEBUG
#pragma comment(lib, "x86/fmtd.lib")
#else
#pragma comment(lib, "x86/fmt.lib")
#endif
#endif