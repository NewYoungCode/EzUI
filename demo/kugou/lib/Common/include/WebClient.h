#pragma once
#include "General.h"

#if USECURL
//ȫ�ֳ�ʼ��curl
void Curl_Global_Init();

//���ؽ��Ȼص�����ģ��
typedef std::function<void(__int64 total, __int64 now, float rate)> ProgressFunc;
//curl�ĳ�ʼ��
static bool g_curl_bInit = false;
class Proxy {
public:
	std::string host;
	unsigned int port;
	int curl_proxytype = 5;//����ֵ�μ�curl_proxytype
	std::string user;
	std::string password;
	inline Proxy(const std::string& host, size_t port, const int& curl_proxytype = 5, const std::string& user = "", const std::string& password = "") {
		this->host = host;
		this->port = port;
		this->curl_proxytype = curl_proxytype;
		this->user = user;
		this->password = password;
	}
};
namespace PostForm {
	//�ֶ�����
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
	//g_curl_receive_callback(char* contents, size_t size, size_t nmemb, void* respone)
	std::function<size_t(char* contents, size_t size, size_t nmemb, void* respone)> CallBack=NULL;
	std::string *ResponseData=NULL;
private:
	void* Init(const std::string& url, std::string& resp, int timeOut);
	long CleanUp(void* curl, int code);
	std::map<std::string, std::string> Header;
	void* curl_header = NULL;//���Ͳμ� curl_slist
public:
	std::string Cookies;
	std::string Proxy_Str;
	WebClient();
	virtual ~WebClient();
	Proxy* Proxy = NULL;
	void AddHeader(const std::string& key, const std::string& value);
	void RemoveHeader(const std::string& key);
	int DownloadFile(const std::string& strUrl, const std::string& filename, const ProgressFunc& progressCallback = NULL, int nTimeout = 20);
	int HttpGet(const std::string& strUrl, std::string& strResponse, int nTimeout = 20);
	int HttpPost(const std::string& strUrl, const std::string& data, std::string& respone, int nTimeout = 20);
	int SubmitForm(const std::string& strUrl, const std::vector<PostForm::Field>& fieldValues, std::string& respone, int nTimeout = 20);
	int UploadFile(const std::string& strUrl, const std::string& filename, const std::string& field, std::string& respone, const ProgressFunc& progressCallback = NULL, int nTimeout = 30);
	int FtpDownLoad(const std::string& strUrl, const std::string& user, const std::string& pwd, const std::string& outFileName, int nTimeout = 30);
};
#endif