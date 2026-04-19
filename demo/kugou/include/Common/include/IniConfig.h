#pragma once
#include "Text.h"
#include <mutex>
#include <functional>

//ini文件操作类
class IniConfig {
private:
	std::wstring m_filename;
	std::mutex m_mtx;
	bool WithIniReadOnly(const std::function<void(void*)>& op);
	bool WithIni(const std::function<void(void*)>& op);
public:
	//ini文件绝对路径
	IniConfig(const Text::String& filename);
	//获取所有的Section名称
	std::vector<Text::String> GetSections();
	//删除指定的Section
	void DeleteSection(const Text::String& section);
	//获取指定Section下的所有Key名称
	std::vector<Text::String> GetKeys(const Text::String& section);
	//删除指定Section下的指定的Key
	void DeleteKey(const Text::String& section, const Text::String& key);
public:
	Text::String ReadString(const Text::String& section, const Text::String& key, const Text::String& defaultValue = "");
	float ReadFloat(const Text::String& section, const Text::String& key, float defaultValue = 0);
	int ReadInt(const Text::String& section, const Text::String& key, int defaultValue = 0);
	bool ReadBool(const Text::String& section, const Text::String& key, bool defaultValue = false);
public:
	bool WriteString(const Text::String& section, const Text::String& key, const Text::String& value);
	bool WriteFloat(const Text::String& section, const Text::String& key, float value);
	bool WriteInt(const Text::String& section, const Text::String& key, int value);
	bool WriteBool(const Text::String& section, const Text::String& key, bool value);
};