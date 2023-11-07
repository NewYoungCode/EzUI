#pragma once
#include "UIDef.h"
namespace EzUI {
	class UI_EXPORT ZipResource {
	private:
		void* handle = NULL;
		std::mutex mtx;
		ZipResource(const ZipResource&) = delete;
		DWORD errCode = 0;
	public:
		ZipResource(const std::wstring& fileName, const std::string& pwd = "");
		ZipResource(const void* fileData, unsigned int fileSize, const std::string& pwd = "");
		bool GetResource(const std::string& itemName, std::string* outData);
		bool IsValid();
		virtual ~ZipResource();
	};
};

