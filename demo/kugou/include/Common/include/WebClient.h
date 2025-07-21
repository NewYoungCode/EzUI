#pragma once
#include <string>
#include <Windows.h>
#include <functional>
#include <mutex>
#include <map>
#include "FileSystem.h"
#include "Text.h"

#define USECURL 1 //是否使用curl 使用curl会导致库变得很大

#if USECURL
namespace PostForm {
	//字段类型
	enum FieldType :char
	{
		None,
		Text,
		File
	};
	class Field {
	public:
		FieldType FieldType = FieldType::None;
		std::string FieldName;
		std::string FieldValue;

		std::string FileName;
		Field(const std::string& FieldName, const std::string& ValueOrFullFileName, PostForm::FieldType FieldType = PostForm::FieldType::Text) {
			this->FieldName = FieldName;
			this->FieldType = FieldType;
			if (FieldType == PostForm::FieldType::File) {
				this->FieldValue = Path::GetFileName(ValueOrFullFileName);
				this->FileName = ValueOrFullFileName;
			}
			else {
				this->FieldValue = ValueOrFullFileName;
			}
		}
	};
}

class WebClient
{
public:
	struct Content
	{
		void* tag = NULL;
		bool cancel = false;
		int type = 0;//0:正常请求 1:下载文件
	};
private:
	Content content;
public:
	std::function<size_t(char* contents, size_t size, size_t nmemb, void* respone)> CallBack = NULL;
private:
	void* Init(const std::string& url, std::string* resp, int timeOut);
	long CleanUp(void* curl, int code);
	std::map<std::string, std::string> Header;
	void* curl_header = NULL;//类型参见 curl_slist
public:
	std::string Cookies;
	std::string Proxy;
	WebClient();
	virtual ~WebClient();
	//取消请求/下载
	void Cancel();
	void AddHeader(const std::string& key, const std::string& value);
	void RemoveHeader(const std::string& key);
	int HttpGet(const std::string& strUrl, std::string* response = NULL, int nTimeout = 60);
	int HttpPost(const std::string& strUrl, const std::string& data = "", std::string* response = NULL, int nTimeout = 60);
	int DownloadFile(const std::string& strUrl, const std::wstring& filename, const std::function<void(long long dltotal, long long dlnow)>& progressCallback = NULL, int nTimeout = 99999);
	int FtpDownLoad(const std::string& strUrl, const std::string& user, const std::string& pwd, const std::wstring& outFileName, int nTimeout = 99999);
	int UploadFile(const std::string& strUrl, const std::string& filename, const std::string& field, std::string* response = NULL, const std::function<void(long long dltotal, long long dlnow)>& progressCallback = NULL, int nTimeout = 99999);
	int SubmitForm(const std::string& strUrl, const std::vector<PostForm::Field>& fieldValues, std::string* response = NULL, int nTimeout = 99999);
};
#endif