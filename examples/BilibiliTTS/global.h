#pragma once
#include "EzUI.h"
#include "VLayout.h"
#include "HLayout.h" 
#include "Form.h" 
#include "Button.h"
#include "VList.h"
#include "TextBox.h"
#include "UIManager.h"
#include <thread>

#include "os.h"
#include "WebClient.h"
#include "JsonCpp.h"

#include <sapi.h>
#pragma comment(lib,"ole32.lib") //CoInitialize CoCreateInstance��Ҫ����ole32.dll 
#pragma comment(lib,"sapi.lib") //sapi.lib��SDK��libĿ¼,������ȷ���� 
using namespace EzUI;

extern ISpVoice* pVoice;
namespace TTS {
	void Speak(const EString& text);
}