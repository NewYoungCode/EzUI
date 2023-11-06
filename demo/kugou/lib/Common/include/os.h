#pragma once
#include "General.h"
struct StopWatch {
private:
	std::chrono::system_clock::time_point beg_t;
public:
	StopWatch() {
		beg_t = std::chrono::system_clock::now();    //
	}
	static time_t Now() {
		auto nowTime = std::chrono::system_clock::now();    //
		return std::chrono::system_clock::to_time_t(nowTime);
	}
	inline time_t ElapsedMilliseconds() {
		auto end_t = std::chrono::system_clock::now();    //
		std::chrono::duration<double> diff = end_t - beg_t;//
		return std::chrono::duration_cast<std::chrono::milliseconds>(diff).count();//
	}
};
namespace Log {
	inline void LogA(const std::string& text) {
		OutputDebugStringA(text.c_str());
		OutputDebugStringA(("\n"));
		std::string appFilename = Path::StartFileName();
		std::string appName = Path::GetFileNameWithoutExtension(appFilename);
#ifdef _DEBUG
		//������ڵ�ǰĿ¼
		std::string dir = Path::GetDirectoryName(appFilename) + "\\" + appName + "_Log";
#else
		//�������TempĿ¼
		CHAR buf[MAX_PATH]{ 0 };
		::GetTempPathA(MAX_PATH, buf);
		std::string dir = std::string(buf) + "\\" + appName + "_Log";
#endif
		std::string logFilename = dir + "/" + Time::Now::ToString("yyyy-MM-dd") + ".log";
		Path::Create(dir);
		std::string logText = Time::Now::ToString("hh:mm:ss ") + text;
		std::ofstream file(logFilename.c_str(), std::ios::app);
		printf("%s\n", logText.c_str());
		file << logText.c_str() << std::endl;
		file.flush();
		file.close();
		//ɾ���������־ ֻ�����������־
	/*	auto today = Time::Now::ToString("yyyy-MM-dd");
		auto files = Path::SearchFiles(dir, "*.log");
		for (auto&it : files) {
			auto t = Path::GetFileNameWithoutExtension(it);
			if (t.size() < 10 || t.substr(0, 10) != today) {
				File::Delete(it);
			}
		}*/
	}

	template<typename ...T>
	void Info(const std::string& formatStr, T... args) {
		int len = formatStr.length();
		int size = 1024 * 1024 * 5;//5M���ڴ�
		char* buf = new char[size] { 0 };
		sprintf_s(buf, size - 1, formatStr.c_str(), std::forward<T>(args)...);
		LogA(buf);
		delete buf;
	}

	template<typename ...T>
	inline void Debug(const std::string& formatStr, T... args) {
#ifdef _DEBUG
		Info(formatStr, args);
#endif
	}

}