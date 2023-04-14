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
//	//::InternetSetOptionA(hintInternetOpen, INTERNET_OPTION_CONNECT_TIMEOUT, TEXT(""), NULL);//设置超时时
//	HINTERNET hintInternetOpenUrl = InternetOpenUrl(hintInternetOpen, url.c_str(), headStr.c_str(), headStr.size(), INTERNET_FLAG_RELOAD, 0);
//	if (!hintInternetOpenUrl)
//	{
//		InternetCloseHandle(hintInternetOpen);
//		InternetCloseHandle(hintInternetOpenUrl);
//		return 1;
//	}
//	//获取head
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
//		// 响应头
//		"*/*"
//	};
//	char szHeader[] =
//	{
//		// 如果提交的是表单,那么这个 MIME 一定要带!
//		"Content-Type: application/x-www-form-urlencoded/r/n"
//	};
//	// 需要提交的数据就放下面这个变量
//	char szPostData[] = "reginvcode=1b1733d743295385&action=reginvcodeck";
//	// 寂寞党可以修改一下 UserAgent 哈哈,我喜欢 Chrome !
//	HINTERNET hInet = InternetOpen("Mozilla/4.0 (Compatible; MSIE 6.0;)", INTERNET_OPEN_TYPE_DIRECT, NULL, INTERNET_INVALID_PORT_NUMBER, 0);
//	// 第二个参数是主机的地址
//	HINTERNET hConn = InternetConnect(hInet, "66.96.216.167", INTERNET_DEFAULT_HTTP_PORT, "", "", INTERNET_SERVICE_HTTP, 0, 1);
//	// 第三个参数是 URL 的路径部分 你懂的,第五个参数是Referer,有些站判断来源地址,修改这里就好啦
//	HINTERNET hPOSTs = HttpOpenRequest(hConn, "POST", "/register.php", HTTP_VERSION, "http://66.96.216.167/", lpszAccept, INTERNET_FLAG_DONT_CACHE, 1);
//	BOOL bRequest = HttpSendRequest(hPOSTs, szHeader, lstrlen(szHeader), szPostData, lstrlen(szPostData));
//	// 不需要接受回应的忽略下面的东东...
//	char szBuffer[1024];
//	DWORD dwByteRead = 0;
//	// 防止乱码的方法就是建立完变量立即清空
//	ZeroMemory(szBuffer, sizeof(szBuffer));
//	// 循环读取缓冲区内容直到结束
//	while (InternetReadFile(hPOSTs, szBuffer, sizeof(szBuffer), &dwByteRead) && dwByteRead > 0) {
//		// 加入结束标记
//		szBuffer[dwByteRead] = '/0';
//		// 应该用变长字符串的 比如 AnsiString
//
//		// 清空缓冲区以备下一次读取
//		ZeroMemory(szBuffer, sizeof(szBuffer));
//
//
//	}
//	// 清理现场
//	InternetCloseHandle(hPOSTs);
//	InternetCloseHandle(hConn);
//	InternetCloseHandle(hInet);
//}