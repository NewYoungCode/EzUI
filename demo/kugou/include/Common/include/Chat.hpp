//#include "os.h"
//#include "WebClient.h"
//
//inline void Loop() {
//	/*Socket s;
//	s.Bind("0.0.0.0", 80);
//	bool b = s.Listen();
//	for (; b;) {
//		Socket client = s.Accep();
//		printf("连接进入 %s %d\n", client.Address.c_str(), client.Port);
//
//		std::string resp;
//		size_t pos = -1;
//		size_t length = 0;
//		std::string body;
//		bool end = false;
//		for (;;) {
//			char buf[128]{ 0 };
//			int len = client.Receive(buf, sizeof(buf));
//			if (len == -1 || len == 0) {
//				client.Close();
//				printf("客户端断开 %s %d\n", client.Address.c_str(), client.Port);
//				break;
//			}
//			if (!end) {
//				resp.append(buf, len);
//			}
//			else {
//				body.append(buf, len);
//			}
//			if (!end && (pos = resp.find("\r\n\r\n")) != -1) {
//				body.append(resp.c_str() + pos + 4, resp.size() - pos - 4);
//				end = true;
//				auto pos2 = resp.find("Content-Length:");
//				if (pos2 != -1) {
//					std::string len;
//					for (size_t i = pos2 + 7; i < resp.size(); i++)
//					{
//						if (resp.at(i) >= '0' && resp.at(i) <= '9') {
//							len += resp.at(i);
//						}
//						else {
//							length = std::atoi(len.c_str());
//							break;
//						}
//					}
//				}
//			}
//			if (end && body.size() >= length) {
//				client.Close();
//				printf("服务端断开 %s %d\n", client.Address.c_str(), client.Port);
//				break;
//			}
//		}
//		printf("%s\n", body.c_str());
//	}*/
//}
//
//int main(int argc, char* argv[])
//{
//
//	StopWatch sw;
//	for (size_t i = 0; i < 15; i++)
//	{
//		WebClient wc;
//		std::string resp;
//		std::string key = "var  items=";
//		size_t pos1 = size_t(-1), pos2 = size_t(-1);
//		wc.CallBack = ([=, &pos1, &pos2](char* contents, size_t size, size_t nmemb, void* respone)->size_t {
//			size_t count = size * nmemb;
//			std::string* str = (std::string*)respone;
//			(*str).append(contents, count);
//			if (pos1 == size_t(-1)) {
//				pos1 = str->find(key);
//			}
//			if (pos1 != size_t(-1)) {
//				pos2 = str->find("}];", key.size() + pos1);
//				if (pos2 != size_t(-1)) {
//					*str = str->substr(pos1 + key.size(), pos2 - (pos1 + key.size())) + "}]";
//					return 23;//终止
//				}
//			}
//			return count;
//			});
//		wc.HttpGet("http://163.197.44.13/Web/reserve/materialx?languages=1", resp, 999);
//	}
//	auto time = sw.ElapsedMilliseconds();
//	return 0;
//}
