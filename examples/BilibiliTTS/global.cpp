#include "global.h"



#include <sapi.h>
#pragma comment(lib,"ole32.lib") //CoInitialize CoCreateInstance��Ҫ����ole32.dll 
#pragma comment(lib,"sapi.lib") //sapi.lib��SDK��libĿ¼,������ȷ���� 
void TTS::Speak(const EString& text)
{
	ISpVoice* pVoice = NULL;
	//��ȡISpVoice�ӿڣ� 
	HRESULT hr = CoCreateInstance(CLSID_SpVoice, NULL, CLSCTX_ALL, IID_ISpVoice, (void**)&pVoice);
	if (SUCCEEDED(hr))
	{
		hr = pVoice->Speak(text.utf16().c_str(), 0, NULL);
		pVoice->Release();
		pVoice = NULL;
	}
}
