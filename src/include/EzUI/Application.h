#pragma once
#include "Window.h"
namespace EzUI {
	class UI_EXPORT Application
	{
		void Init();
	public:
		static void Exit(int exitCode = 0) {
			::PostQuitMessage(exitCode);
		}
	public:
		Application();
		//使用vs中的资源ID加载资源包 resID资源ID custResType自定义资源类型名称 密码
		Application(int resID, const EString& custResType, const EString& password = "");
		//使用本地文件名称加载资源包
		Application(const EString& fileName, const EString& password = "");
		//启用高DPI适配
		void EnableHighDpi();
		virtual ~Application();
		int Exec();
	};
};