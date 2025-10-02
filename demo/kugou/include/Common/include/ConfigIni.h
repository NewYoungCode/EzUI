#pragma once
#include <Windows.h>
#include "Text.h"

class ConfigIni {
protected:
	size_t buffSize;
	Text::String filename;
	Text::String section;
	DWORD GetValue(const Text::String& key,  const Text::String& filename, Text::String& outResult);
	bool SetValue(const Text::String& key, const Text::String& Value, const Text::String& absoluteFilename);
private:
	ConfigIni() = delete;
	ConfigIni(const ConfigIni&) = delete;
public:
	//FileName //一定要绝对路径
	ConfigIni(const Text::String& filename, const Text::String& defaultSection = "setting", size_t buffSize = 1024);
	//设置一个名称
	void SetSection(const Text::String& sectionName);
	//读取ini中的字符
	Text::String ReadString(const Text::String& key, const Text::String& defaultValue = "");
	//读取ini中的数字
	float ReadFloat(const Text::String& key, float defaultValue = 0);
	//读取ini中的int数字
	int ReadInt(const Text::String& key, int defaultValue = 0);
	//读取bool类型值
	bool ReadBool(const Text::String& key, bool defaultValue = false);
	//写入ini
	bool WriteString(const Text::String& key, const Text::String& value);
	bool WriteFloat(const Text::String& key, float value);
	bool WriteInt(const Text::String& key, int value);
	bool WriteBool(const Text::String& key, bool defaultValue);

	//获取所有的Section
	std::vector<Text::String> GetSections();
	//删除所有的Section
	void DeleteSection(const Text::String& section);
};