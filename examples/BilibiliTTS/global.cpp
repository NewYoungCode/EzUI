#include "global.h"

ISpVoice* pVoice = NULL;
void TTS::Speak(const EString& text)
{
	pVoice->Speak(text.utf16().c_str(), 0, NULL);
}
