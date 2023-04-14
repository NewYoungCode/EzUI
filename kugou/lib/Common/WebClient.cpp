#include "WebClient.h"

#if USECURL

#ifndef CURL_STATICLIB
#define CURL_STATICLIB
#endif // !CURL_STATICLIB
#include "curl/curl.h"
#include "curl/easy.h"
#ifdef _DEBUG
#pragma comment(lib,"libcurld.lib")
#else
#pragma comment(lib,"libcurl.lib")
#endif // !_DEBUG
#pragma comment(lib,"Crypt32.lib")//curl��Ҫ�Ŀ�
#pragma comment(lib,"wldap32.lib")//curl��Ҫ�Ŀ�
#pragma comment(lib,"ws2_32.lib") //curl��Ҫ�Ŀ�

void Curl_Global_Init() {
	curl_global_init(CURL_GLOBAL_ALL);
}

//������Ӧbody
size_t g_curl_receive_callback(char* contents, size_t size, size_t nmemb, void* respone);
//���������ļ�
size_t g_curl_download_callback(char* contents, size_t size, size_t nmemb, void* _fielname);
//�����ϴ��������ؽ���
int g_curl_progress_callback(void* ptr, __int64 dltotal, __int64 dlnow, __int64 ultotal, __int64 ulnow);

//����
WebClient::WebClient() {
	if (!g_curl_bInit) {
		curl_global_init(CURL_GLOBAL_ALL); //��ʼ��curl
		g_curl_bInit = true;
	}
}
WebClient::~WebClient() {
	if (this->Proxy) {
		delete this->Proxy;
	}
}
size_t __g_curl_receive_callback(char* contents, size_t size, size_t nmemb, void* respone) {
	size_t count = size * nmemb;
	std::string* str = (std::string*)respone;
	(*str).append(contents, count);
	//CURLE_WRITE_ERROR
	return count;
};

size_t g_curl_receive_callback(char* contents, size_t size, size_t nmemb, void* respone) {
	WebClient* wc = (WebClient*)respone;
	if (wc->CallBack) {
		return wc->CallBack(contents, size, nmemb, wc->ResponseData);
	}
	return __g_curl_receive_callback(contents, size, nmemb, wc->ResponseData);
};

size_t g_curl_download_callback(char* contents, size_t size, size_t nmemb, void* fielname) {
	std::ofstream  ofs((char*)fielname, std::ios::app | std::ios::binary);
	size_t count = size * nmemb;
	ofs.write(contents, count);
	ofs.flush();
	ofs.close();
	return count;
}
int g_curl_progress_callback(void* ptr, __int64 dltotal, __int64 dlnow, __int64 ultotal, __int64 ulnow)
{
	if (dltotal != 0 && ptr) {
		(*(ProgressFunc*)ptr)(dltotal, dlnow, dlnow / (dltotal * 1.0) * 100);
	}
	return 0;
}
CURL* WebClient::Init(const std::string& strUrl, std::string& strResponse, int nTimeout) {
	strResponse.clear();
	CURL* curl = curl_easy_init();
	if (!curl) {
		return curl;
	}
	this->ResponseData = &strResponse;
	if (Proxy) {
		curl_easy_setopt(curl, CURLOPT_PROXYTYPE, Proxy->curl_proxytype); //����ģʽ
		curl_easy_setopt(curl, CURLOPT_PROXY, Proxy->host.c_str()); //�����������ַ
		curl_easy_setopt(curl, CURLOPT_PROXYPORT, Proxy->port); //����������˿�
		if (!Proxy->user.empty() && !Proxy->password.empty()) {
			curl_easy_setopt(curl, CURLOPT_PROXYUSERPWD, std::string(Proxy->user + ":" + Proxy->password).c_str()); //�û�����
		}
	}
	if (!Proxy_Str.empty()) {
		curl_easy_setopt(curl, CURLOPT_PROXY, Proxy_Str.c_str()); //�����������ַ
	}

	//��ʼ��cookie����
	curl_easy_setopt(curl, CURLOPT_COOKIEFILE, "");    //��ʼ��cookie����,������ȷ���յ�cookie����.
	if (!Cookies.empty()) {
		curl_easy_setopt(curl, CURLOPT_COOKIE, Cookies.c_str());
	}
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);	// ��֤�Է���SSL֤��
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, false);	//����������֤֤�������
	curl_easy_setopt(curl, CURLOPT_URL, strUrl.c_str());//����url��ַ
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, nTimeout);//���ó�ʱ
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, g_curl_receive_callback);//���ܻص�

	//curl_easy_setopt(curl, CURLOPT_WRITEDATA, &strResponse);//
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, this);//

	for (auto& it : Header) {
		auto hd = it.first + ":" + it.second;
		curl_header = curl_slist_append((curl_slist*)curl_header, hd.c_str());
	}
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, (curl_slist*)curl_header);
	return curl;
};
long WebClient::CleanUp(void* curl, int code) {
	long RESPONSE_CODE = (int)code;
	//���ִ�гɹ�,
	if (code == CURLE_OK)
	{
		//��ȡ��Ӧ��״̬��
		curl_easy_getinfo((CURL*)curl, CURLINFO_RESPONSE_CODE, &RESPONSE_CODE);

		struct curl_slist* cookies = NULL;
		curl_easy_getinfo((CURL*)curl, CURLINFO_COOKIELIST, &cookies);       //���cookie����  
		int i = 1;
		while (cookies)
		{
			//printf("%s\n", cookies->data);
			Cookies.append(cookies->data);
			cookies = cookies->next;
			i++;
		}
	}

	if (curl_header) {
		curl_slist_free_all((curl_slist*)curl_header);
		curl_header = NULL;
	}
	curl_easy_cleanup(curl);
	return RESPONSE_CODE;
};
int WebClient::HttpGet(const std::string& strUrl, std::string& strResponse, int nTimeout) {
	CURL* curl = Init(strUrl, strResponse, nTimeout);
	if (!curl) {
		return CURLE_FAILED_INIT;
	}
	CURLcode code = curl_easy_perform(curl);
	return CleanUp(curl, code);
};
int WebClient::HttpPost(const std::string& url, const std::string& data, std::string& respone, int _timeout) {

	CURL* curl = Init(url, respone, _timeout);
	if (!curl) {
		return CURLE_FAILED_INIT;
	}
	curl_easy_setopt(curl, CURLOPT_POST, true);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
	CURLcode code = curl_easy_perform(curl);
	return CleanUp(curl, code);
};
int WebClient::UploadFile(const std::string& url, const std::string& filename, const std::string& field, std::string& respone, const ProgressFunc& progressCallback, int _timeout) {

	CURL* curl = Init(url, respone, _timeout);
	if (!curl) {
		return CURLE_FAILED_INIT;
	}
	struct curl_httppost* formpost = 0;
	struct curl_httppost* lastptr = 0;
	curl_formadd(&formpost, &lastptr, CURLFORM_PTRNAME, field.c_str(), CURLFORM_FILE, filename.c_str(), CURLFORM_END);
	curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);


	if (progressCallback) {
		curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);//�����ϴ����ؽ���
		curl_easy_setopt(curl, CURLOPT_XFERINFODATA, &progressCallback);//�������ص��������ô���ָ��
		curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, g_curl_progress_callback);//���Ȼص�
	}

	CURLcode  code = curl_easy_perform(curl);
	if (formpost) {
		curl_formfree(formpost);
	}


	return CleanUp(curl, code);
};
int WebClient::SubmitForm(const std::string& strUrl, const std::vector<PostForm::Field>& fieldValues, std::string& respone, int nTimeout) {

	AddHeader("Content-Type", "multipart/form-data");
	CURL* curl = Init(strUrl, respone, nTimeout);
	if (!curl) {
		return CURLE_FAILED_INIT;
	}
	struct curl_httppost* formpost = NULL;
	struct curl_httppost* lastptr = NULL;
	// ���ñ�ͷ����ͷ���ݿ��ܲ�ͬ
	for (auto& item : fieldValues) {
		if (item.FieldType == PostForm::FieldType::File) {
			curl_formadd(&formpost, &lastptr,
				CURLFORM_COPYNAME, item.FieldName.c_str(),
				CURLFORM_FILE, item.FileName.c_str(),
				CURLFORM_CONTENTTYPE, "application/octet-stream",
				CURLFORM_END);
		}
		if (item.FieldType == PostForm::FieldType::Text) {
			curl_formadd(&formpost, &lastptr, CURLFORM_COPYNAME, item.FieldName.c_str(), CURLFORM_COPYCONTENTS, item.FieldValue.c_str(), CURLFORM_END);
		}
	}
	// ���ñ�����
	curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);
	CURLcode code = curl_easy_perform(curl);

	if (formpost) {
		curl_formfree(formpost);
	}
	return CleanUp(curl, code);

};
int WebClient::DownloadFile(const std::string& url, const std::string& _filename, const ProgressFunc& progressCallback, int nTimeout) {
	std::string resp;
	CURL* curl = Init(url, resp, nTimeout);
	if (!curl) {
		return CURLE_FAILED_INIT;
	}
	File::Delete(_filename);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, g_curl_download_callback);//�������صĻص�����
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, _filename.c_str()); //�����ļ���

	if (progressCallback) {
		curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);//�����ϴ����ؽ���
		curl_easy_setopt(curl, CURLOPT_XFERINFODATA, &progressCallback);//�������ص��������ô���ָ��
		curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, g_curl_progress_callback);//���Ȼص�
	}

	CURLcode  code = curl_easy_perform(curl);
	return CleanUp(curl, code);
};
int WebClient::FtpDownLoad(const std::string& strUrl, const std::string& user, const std::string& pwd, const std::string& outFileName, int nTimeout) {

	std::string resp;
	CURL* curl = Init(strUrl, resp, nTimeout);
	if (!curl) {
		return CURLE_FAILED_INIT;
	}
	if (!user.empty() && !pwd.empty()) {
		curl_easy_setopt(curl, CURLOPT_USERPWD, (user + ":" + pwd).c_str());
	}
	else {
		curl_easy_setopt(curl, CURLOPT_USERPWD, "");
	}
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, g_curl_download_callback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, outFileName.c_str());
	CURLcode code = curl_easy_perform(curl);
	return CleanUp(curl, code);
}
void WebClient::AddHeader(const std::string& key, const std::string& value)
{
	Header.insert(std::pair<std::string, std::string>(key, value));
}
void WebClient::RemoveHeader(const std::string& key)
{
	Header.erase(key);
};

#endif