#include "global.h"



#include <sapi.h>
#pragma comment(lib,"ole32.lib") //CoInitialize CoCreateInstance需要调用ole32.dll 
#pragma comment(lib,"sapi.lib") //sapi.lib在SDK的lib目录,必需正确配置 
void TTS::Speak(const EString& text)
{
	ISpVoice* pVoice = NULL;
	//获取ISpVoice接口： 
	HRESULT hr = CoCreateInstance(CLSID_SpVoice, NULL, CLSCTX_ALL, IID_ISpVoice, (void**)&pVoice);
	if (SUCCEEDED(hr))
	{
		hr = pVoice->Speak(text.utf16().c_str(), 0, NULL);
		pVoice->Release();
		pVoice = NULL;
	}
}
