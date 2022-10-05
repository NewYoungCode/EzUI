#pragma once
#include "EzUI.h"
#include "VLayout.h"
#include "HLayout.h" 
#include "Form.h" 
#include "Button.h"
#include "VList.h"
#include "Edit.h"
#include "UIManager.h"
#include <thread>

#include "os.h"
#include "WebClient.h"
#include "JsonCpp.h"

#include <sapi.h>
#pragma comment(lib,"ole32.lib") //CoInitialize CoCreateInstance需要调用ole32.dll 
#pragma comment(lib,"sapi.lib") //sapi.lib在SDK的lib目录,必需正确配置 
using namespace ui;

extern ISpVoice* pVoice;
namespace TTS {
	void Speak(const EString& text);
}