#pragma once
#include <iostream>
#include <string>
#include <functional>
#include <fstream>
#include <vector>
#include <sstream>
#include <mutex>
#include <memory>
#include <map>
#include <io.h>
#include <Windows.h>
#include <winuser.h>
#include <assert.h>

#pragma warning(disable:4018)
#pragma warning(disable:4244)
#pragma warning(disable:4551)
#pragma warning(disable:4293)
#pragma warning(disable:4996)
#pragma warning(disable:4099)
#pragma warning(disable:4800)
#pragma warning(disable:4101)
#pragma warning(disable:4267)


#include "Text.h"
#include "FileSystem.h"
#include "Time.hpp"//防止和系统的time.h重命名
#include "WinTool.h"
