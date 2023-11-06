#include "Time.hpp"
namespace Time {
	namespace Now {
		std::string ToString(const std::string &format) {
			SYSTEMTIME time;
			GetLocalTime(&time);
			std::string  year = std::to_string(time.wYear);//��
			std::string  Month = std::to_string(time.wMonth);//��
			std::string  Day = std::to_string(time.wDay);//��
			std::string Hour = std::to_string(time.wHour);//ʱ
			std::string Minute = std::to_string(time.wMinute);//��
			std::string Second = std::to_string(time.wSecond);//��
			std::string wMilliseconds = std::to_string(time.wMilliseconds);//����
			Month = Month.size() == 1U ? "0" + Month : Month;
			Day = Day.size() == 1U ? "0" + Day : Day;
			Hour = Hour.size() == 1U ? "0" + Hour : Hour;
			Minute = Minute.size() == 1U ? "0" + Minute : Minute;
			Second = Second.size() == 1U ? "0" + Second : Second;
			std::string formatStr = format;
			formatStr = Text::ReplaceAll(formatStr, "yyyy", year);
			formatStr = Text::ReplaceAll(formatStr, "MM", Month);
			formatStr = Text::ReplaceAll(formatStr, "dd", Day);
			formatStr = Text::ReplaceAll(formatStr, "hh", Hour);
			formatStr = Text::ReplaceAll(formatStr, "mm", Minute);
			formatStr = Text::ReplaceAll(formatStr, "ss", Second);
			formatStr = Text::ReplaceAll(formatStr, "mmmm", wMilliseconds);
			return formatStr;
		}
	}
}