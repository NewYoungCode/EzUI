#pragma once
#include "General.h"
class ConfigIni {
protected:
	size_t buffSize = MAX_PATH;
	std::string filename;
	std::string section;
	DWORD GetValue(const std::string &section, const std::string &key, const std::string &defaultValue, const std::string &filename, std::string&outResult)const;
	bool SetValue(const std::string & section, const std::string & key, const std::string & Value, const std::string & absoluteFilename)const;
public:
	ConfigIni();
	//FileName //һ��Ҫ����·��
	ConfigIni(const std::string& filename, const std::string &defaultSection = "setting", bool create = true, size_t buffSize = MAX_PATH);
	void SetDefaultSection(const std::string section);
	bool DeleteKey(const std::string &key, const std::string &section = "");
	//��ȡini�е��ַ�
	std::string ReadString(const std::string &key, const std::string& defaultValue = "", const std::string &section = "") const;
	//��ȡini�е�����
	float ReadFloat(const std::string &key, float defaultValue = 0, const std::string &section = "") const;
	//��ȡini�е�int����
	int ReadInt(const std::string &key, int defaultValue = 0, const std::string &section = "") const;
	//д��ini
	bool WriteValue(const std::string &key, const std::string &value, const std::string &section = "")const;
	std::vector<std::string> GetSections();
	void DeleteSection(const std::string&section);
};

class SafeConfigIni :public ConfigIni {
private:
	std::string oldFilename;
public:
	//����
	static void EnCode(const std::string &filename, const std::string &outFilename) {
		std::string data;
		File::ReadFile(filename, data);
		char *memBytes = new char[data.size()];
		size_t pos = 0;
		for (auto&it : data) {
			memBytes[pos] = it + 1;
			pos++;
		}
		File::Delete(outFilename);
		std::ofstream ofs(outFilename, std::ios::app | std::ios::binary);
		ofs.write(memBytes, data.size());
		ofs.flush();
		ofs.close();
		delete memBytes;
	}
	//����
	static void DeCode(const std::string &filename, const std::string &outFilename) {
		std::string data;
		File::ReadFile(filename, data);
		char *memBytes = new char[data.size()];
		size_t pos = 0;
		for (auto&it : data) {
			memBytes[pos] = it - 1;
			pos++;
		}
		File::Delete(outFilename);
		std::ofstream ofs(outFilename, std::ios::app | std::ios::binary);
		ofs.write(memBytes, data.size());
		ofs.flush();
		ofs.close();
		delete memBytes;
	}

	SafeConfigIni(const std::string &filename, const std::string &defaultSection = "setting", bool create = true, size_t buffSize = MAX_PATH);
};