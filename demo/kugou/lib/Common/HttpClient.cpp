//#include "HttpClient.h"
//void HttpClient::AddHeader(const std::string&key, const std::string&value) {
//	Headers.insert(std::pair<std::string, std::string>(key, value));
//}
//
//HttpClient::~HttpClient() {}
//HttpClient::HttpClient() {
//
//	AddHeader("Accept", "*/*");
//	AddHeader("Accept-Encoding", "gzip, deflate");
//}
//int HttpClient::Get(const std::string & url, std::string& outRespone)
//{
//	outRespone.clear();
//	HINTERNET hintInternetOpen = InternetOpen(TEXT("Testing"), INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
//	if (!hintInternetOpen)
//	{
//		InternetCloseHandle(hintInternetOpen);
//		return 1;
//	}
//
//	std::string headStr = "";
//	for (auto&it : Headers) {
//		headStr.append(it.first + ": " + it.second+"\r\n");
//	}
//	headStr.append("\r\n\r\n");
//	//::InternetSetOptionA(hintInternetOpen, INTERNET_OPTION_CONNECT_TIMEOUT, TEXT(""), NULL);//���ó�ʱʱ
//	HINTERNET hintInternetOpenUrl = InternetOpenUrl(hintInternetOpen, url.c_str(), headStr.c_str(), headStr.size(), INTERNET_FLAG_RELOAD, 0);
//	if (!hintInternetOpenUrl)
//	{
//		InternetCloseHandle(hintInternetOpen);
//		InternetCloseHandle(hintInternetOpenUrl);
//		return 1;
//	}
//	//��ȡhead
//	DWORD dwByteSize = 0;
//	DWORD dwSizeOfRq = 4;
//	if (!HttpQueryInfoA(hintInternetOpenUrl, HTTP_QUERY_CONTENT_LENGTH | HTTP_QUERY_FLAG_NUMBER, (LPVOID)&dwByteSize, &dwSizeOfRq, NULL))
//	{
//		DWORD dwErr = ::GetLastError();
//		dwByteSize = 0;
//	}
//
//	while (true)
//	{
//		DWORD dwByteRead = 0;
//		char szDownBuffer[256]{ 0 };
//		auto ret = InternetReadFile(hintInternetOpenUrl, szDownBuffer, 256, &dwByteRead);
//		if (!dwByteRead)
//		{
//			break;
//		}
//		outRespone.append(szDownBuffer, dwByteRead);
//	}
//	InternetCloseHandle(hintInternetOpen);
//	InternetCloseHandle(hintInternetOpenUrl);
//	return 0;
//}
//void HttpClient::Post(const std::string&url, std::string&respone) {
//	LPCSTR lpszAccept[] =
//	{
//		// ��Ӧͷ
//		"*/*"
//	};
//	char szHeader[] =
//	{
//		// ����ύ���Ǳ�,��ô��� MIME һ��Ҫ��!
//		"Content-Type: application/x-www-form-urlencoded/r/n"
//	};
//	// ��Ҫ�ύ�����ݾͷ������������
//	char szPostData[] = "reginvcode=1b1733d743295385&action=reginvcodeck";
//	// ��į�������޸�һ�� UserAgent ����,��ϲ�� Chrome !
//	HINTERNET hInet = InternetOpen("Mozilla/4.0 (Compatible; MSIE 6.0;)", INTERNET_OPEN_TYPE_DIRECT, NULL, INTERNET_INVALID_PORT_NUMBER, 0);
//	// �ڶ��������������ĵ�ַ
//	HINTERNET hConn = InternetConnect(hInet, "66.96.216.167", INTERNET_DEFAULT_HTTP_PORT, "", "", INTERNET_SERVICE_HTTP, 0, 1);
//	// ������������ URL ��·������ �㶮��,�����������Referer,��Щվ�ж���Դ��ַ,�޸�����ͺ���
//	HINTERNET hPOSTs = HttpOpenRequest(hConn, "POST", "/register.php", HTTP_VERSION, "http://66.96.216.167/", lpszAccept, INTERNET_FLAG_DONT_CACHE, 1);
//	BOOL bRequest = HttpSendRequest(hPOSTs, szHeader, lstrlen(szHeader), szPostData, lstrlen(szPostData));
//	// ����Ҫ���ܻ�Ӧ�ĺ�������Ķ���...
//	char szBuffer[1024];
//	DWORD dwByteRead = 0;
//	// ��ֹ����ķ������ǽ���������������
//	ZeroMemory(szBuffer, sizeof(szBuffer));
//	// ѭ����ȡ����������ֱ������
//	while (InternetReadFile(hPOSTs, szBuffer, sizeof(szBuffer), &dwByteRead) && dwByteRead > 0) {
//		// ����������
//		szBuffer[dwByteRead] = '/0';
//		// Ӧ���ñ䳤�ַ����� ���� AnsiString
//
//		// ��ջ������Ա���һ�ζ�ȡ
//		ZeroMemory(szBuffer, sizeof(szBuffer));
//
//
//	}
//	// �����ֳ�
//	InternetCloseHandle(hPOSTs);
//	InternetCloseHandle(hConn);
//	InternetCloseHandle(hInet);
//}