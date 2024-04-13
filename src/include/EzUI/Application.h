#pragma once
#include "Window.h"
#include "Resource.h"
namespace EzUI {
	class UI_EXPORT Application
	{
		void Init();
	public:
		//退出消息循环
		static void Exit(int exitCode = 0);
		//获取在vs中存在的资源文件
		static bool GetResource(int resId, const EString& resType, std::string* outData);
	public:
		Application();
		//使用vs中的资源ID加载资源包 resID资源ID custResType自定义资源类型名称 密码
		Application(int resID, const EString& custResType);
		//使用本地文件名称加载资源包
		Application(const EString& fileName);
		//启用高DPI适配
		void EnableHighDpi();
		virtual ~Application();
		int Exec();
	};
};