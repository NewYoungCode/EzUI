#pragma once	
#include "core/UIDef.h"
#include "graphics/GraphicsTypes.h"
#include "utils/String.h"
namespace ezui {

	// 定义用于保存显示器信息的结构体
	struct DisplayInfo {
		//显示器缩放比例 1.0 1.25 1.5 1.75 2.0
		float Scale = 1.0f;
		//显示器帧率
		float FPS = 60;
		//是否为主显示器
		bool Primary = false;
		//显示器名称
		UIString DisplayName;
		HMONITOR Monitor = NULL;
		//显示器物理宽高
		Size Physical;
		//显示器的位置 多显示器下Y轴可能出现负数或者大于0的时候代表显示器在设置里面显示器是错位的(多显示器没有平行);
		//逻辑宽高
		ezui::Rect Rect;
		//工作区域
		ezui::Rect WorkRect;
		//显示是否可用
		bool IsValid() const {
			return Monitor != NULL;
		}
	};

	//判断两个float是相等(两数是否接近)
	inline bool IsFloatEqual(float num1, float num2) { return std::fabsf(num1 - num2) <= EZUI_FLOAT_EPSILON; }
	// 获取与系统时间无关的单调递增的毫秒时间戳(基于std::chrono::steady_clock)
	inline int64_t GetTickCountMs() { return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count(); }
	// 睡眠指定毫秒数，并返回实际睡眠的毫秒数
	inline int64_t SleepMs(int64_t ms) {
		if (ms <= 0) return 0;
		auto start = GetTickCountMs();
		std::this_thread::sleep_for(std::chrono::milliseconds(ms));
		return GetTickCountMs() - start;
	}

	//复制内容到剪切板
	extern EZUI_API bool CopyToClipboard(int uFormat, void* pData, size_t size, WindowHandle hWnd = NULL);
	//打开剪切板
	extern EZUI_API bool GetClipboardData(int uFormat, std::function<void(void*, size_t)> Callback, WindowHandle hWnd = NULL);
	//复制unicode文字
	extern EZUI_API bool CopyToClipboard(const std::wstring& str, WindowHandle hWnd = NULL);
	//粘贴unicode文字
	extern EZUI_API bool GetClipboardData(std::wstring* outStr, WindowHandle hWnd = NULL);
	//获取当前所有监视器的信息
	extern EZUI_API std::list<DisplayInfo> GetDisplays();
	//使用窗口句柄获取用户当前所在的显示器
	extern EZUI_API DisplayInfo GetDisplay(WindowHandle hWnd);
	//使用鼠标位置获取用户当前所在的显示器
	extern EZUI_API DisplayInfo GetDisplay(const Point& mousePoint);
	//使用窗口的矩形位置获取所在的显示器
	extern EZUI_API DisplayInfo GetDisplay(const Rect& rect);
	//获取主显示器的刷新率
	extern EZUI_API float GetPrimaryDisplayRefreshRate();

	namespace detail {
		//内部用于计算内容大小的工具
		class EZUI_API ContentMeasure {
		private:
			int m_minLeft, m_maxRight;
			int m_minTop, m_maxBottom;
		public:
			ContentMeasure()noexcept;
			void Update(int left, int top, int right, int bottom)noexcept;
			void Update(const Rect& rect, const Distance& margin)noexcept;
			int ContentWidth() const noexcept;
			int ContentHeight() const noexcept;
		};
	};
	//统计函数耗时
	template<class Func, class... Args>
	int64_t StopWatch(Func&& f, Args&& ...args) {
		auto beginTime = ezui::GetTickCountMs();
		std::forward<Func>(f)(std::forward<Args>(args)...);
		auto delta = ezui::GetTickCountMs() - beginTime;
		return delta;
	}
};