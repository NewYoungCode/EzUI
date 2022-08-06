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

namespace TTS {
	void Speak(const EString& text);
}